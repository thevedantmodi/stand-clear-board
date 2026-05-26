#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_http_client.h"
#include "esp_netif.h"

#include "esp_crt_bundle.h"

#include "esp_wpa2.h"

#include "driver/uart.h"

#define USE_TUFTS_WIFI

/*
TODO
change UART protocol with shayne to have start sentinel byte in case the 6 bytes get misaligned
*/

#define TRAIN_BUFF_SIZE 64
char train_data_buff[TRAIN_BUFF_SIZE];
int train_data_buff_len = 0;

const char *train_lines[] = {"1", "2", "3", "4", "5", "6", "7", "A", "B", "C", "D", "E", "F", "G", "J", "L", "M", "N", "Q", "R", "W", "Z"};

static const char *WIFI_TAG = "WIFI";
static const char *UART_TAG = "UART";

// port to communicate with user input STM
#define UART_RX_PORT UART_NUM_1
#define RX_PIN 16

// port to communicate with screen STM
#define UART_TX_PORT UART_NUM_2
#define TX_PIN 17

#define BUF_SIZE 1024

static QueueHandle_t uart_queue;

typedef struct {
    uint16_t station_index;
    char lines[4];
} user_request;
static_assert(sizeof(user_request) == 6);

QueueHandle_t msg_queue;

void uart_rx_init(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_RX_PORT, &uart_config);

    // Set TX/RX pins
    uart_set_pin(UART_RX_PORT, UART_PIN_NO_CHANGE, RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver (allocates buffers)
    ESP_ERROR_CHECK(uart_driver_install(UART_RX_PORT, BUF_SIZE, 0,
                        10, &uart_queue, 0));
}

void uart_tx_init(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_TX_PORT, &uart_config);

    // Set TX/RX pins
    uart_set_pin(UART_TX_PORT, TX_PIN, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver (allocates buffers)
    ESP_ERROR_CHECK(uart_driver_install(UART_TX_PORT, 256, 0, 0, NULL, 0));
}

void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t data[BUF_SIZE];

    user_request req;

    while (1) {
        // Wait for UART event (triggered underneath by interrupt)
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {

            switch (event.type) {

            case UART_DATA:
                // Data received
                int len = uart_read_bytes(UART_RX_PORT, data,
                                          event.size, portMAX_DELAY);

                ESP_LOGI(UART_TAG, "Len recvd: %d", len);
                for (int i = 0; i < len; i++) {
                    ESP_LOGI(UART_TAG, "Character num: %d %c", data[i], data[i]);
                }

                if (len == 6) {
                    ESP_LOGI(UART_TAG, "Full transmission of 6 received");
                    memcpy(&req, data, len);
                    req.station_index = ntohs(req.station_index);

                    if (xQueueSend(msg_queue, &req, pdMS_TO_TICKS(100))) {
                        ESP_LOGI(UART_TAG, "Sent msg to queue");
                    } else {
                        ESP_LOGW(UART_TAG, "Queue full, message dropped");
                    }
                }

                break;

            case UART_FIFO_OVF:
                ESP_LOGW(UART_TAG, "FIFO overflow");
                uart_flush_input(UART_RX_PORT);
                xQueueReset(uart_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGW(UART_TAG, "Buffer full");
                uart_flush_input(UART_RX_PORT);
                xQueueReset(uart_queue);
                break;

            default:
                break;
            }
        }
    }
}

// ===== WiFi credentials =====
#ifdef USE_TUFTS_WIFI
#define WIFI_SSID "Redacted"
#define WIFI_USERNAME "Redacted"
#define WIFI_PASS "Redacted"

#else
#define WIFI_SSID "Redacted"
#define WIFI_PASS "Redacted"
#endif

// ===== Event group =====
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

// ===== WiFi event handler =====
static void event_handler(void *arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(WIFI_TAG, "WiFi disconnected, retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(WIFI_TAG, "Got IP address!");
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// ===== WiFi init =====

void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    #ifdef USE_TUFTS_WIFI

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            // password is not used for enterprise here
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Enable WPA2 Enterprise
    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)WIFI_USERNAME, strlen(WIFI_USERNAME)));
    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_username((uint8_t *)WIFI_USERNAME, strlen(WIFI_USERNAME)));
    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_password((uint8_t *)WIFI_PASS, strlen(WIFI_PASS)));

    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable());

    #else

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    #endif

    ESP_ERROR_CHECK(esp_wifi_start());
}

// ===== HTTP event handler =====
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        // printf("\nNEXT CHUNK OF SIZE %d\n", evt->data_len);

        if (evt->data_len + train_data_buff_len < TRAIN_BUFF_SIZE) {
            memcpy(train_data_buff + train_data_buff_len, evt->data, evt->data_len);
            train_data_buff_len += evt->data_len;
            // TODO check if it's error response?

            if (train_data_buff_len == 12) {
                // Full response received
                for (int i = 0; i < 12; i += 3) {
                    if (train_data_buff[i + 1] != 0xFF && train_data_buff[i + 2] != 0xFF) {
                        int minutes = train_data_buff[i];
                        const char* line = train_lines[(int)(train_data_buff[i + 1])];
                        bool direction_south = train_data_buff[i+2];
                        printf("NEXT3: %d %d %d\n", train_data_buff[i], train_data_buff[i+1], train_data_buff[i+2]);

                        printf("NEXT TRAIN: %d %s %s\n", minutes, line, direction_south ? "south" : "north");
                    } else {
                        printf("NO TRAIN\n");
                    }
                }

                int len_sent = uart_write_bytes(UART_TX_PORT, train_data_buff, 12);
                ESP_LOGI(UART_TAG, "%d bytes sent", len_sent);
            } else if (train_data_buff_len > 12) {
                // Something went wrong
                train_data_buff_len = 0;
            }
        } else {
            printf("OVERFLOW, CHUNK TOO BIG\n");
        }
    }
    return ESP_OK;
}

// ===== HTTP GET task =====
void http_get_task(void *pvParameters)
{
    bool req_initialized = false;
    user_request new_req;
    user_request latest_req;

    while (true) {
        if (xQueueReceive(msg_queue, &new_req, pdMS_TO_TICKS(15000))) {
            latest_req = new_req;
            req_initialized = true;
            ESP_LOGI(UART_TAG, "HTTP task received msg from queue");
        } else { // Timeout
            if (!req_initialized) {
                printf("NO DATA YET\n");
                continue; // keep waiting till first user request received
            }
        }

        char url_string[256];
        for (int i = 0; i < 4; i++) {
            if (latest_req.lines[i] == '\0')
                latest_req.lines[i] = '0';
        }
        sprintf(url_string,
                "http://tezan-130-64-64-33.run.pinggy-free.link/arrivals?station=%d&line=%c&line=%c&line=%c&line=%c",
                latest_req.station_index, latest_req.lines[0], latest_req.lines[1], latest_req.lines[2], latest_req.lines[3]);

        esp_http_client_config_t config = {
            .event_handler = http_event_handler,
            .crt_bundle_attach = esp_crt_bundle_attach,
        };
        config.url = url_string;

        esp_http_client_handle_t client = esp_http_client_init(&config);

        ESP_LOGI(WIFI_TAG, "Performing HTTP GET...");
        train_data_buff_len = 0;
        bzero(train_data_buff, TRAIN_BUFF_SIZE);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) {
            ESP_LOGI(WIFI_TAG, "HTTP Status = %d",
                     esp_http_client_get_status_code(client));
        } else {
            ESP_LOGE(WIFI_TAG, "HTTP request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }

    vTaskDelete(NULL);
}

// void fake_uart_task(void *pvParameters)
// {
//     user_request req;
//     int counter = 0;

//     vTaskDelay(pdMS_TO_TICKS(20000));

//     while (1) {
//         // Populate message with changing data
//         req.station_index = counter++ % 300;
//         req.lines[0] = '1';
//         req.lines[1] = '6';
//         req.lines[2] = 'E';
//         req.lines[3] = 'J';

//         // Send to queue (wait up to 100 ms if full)
//         if (xQueueSend(msg_queue, &req, pdMS_TO_TICKS(100))) {
//             ESP_LOGI("FAKE_UART", "Sent: %d",
//                      req.station_index);
//         } else {
//             ESP_LOGW("FAKE_UART", "Queue full, dropping message");
//         }

//         // Wait a few seconds before sending next message
//         vTaskDelay(pdMS_TO_TICKS(3000));
//     }
// }

// ===== Main app =====
void app_main(void)
{
    msg_queue = xQueueCreate(10, sizeof(user_request));
    if (msg_queue == NULL) {
        ESP_LOGE("APP", "Failed to create queue");
        return;
    }

    uart_rx_init();
    uart_tx_init();

    esp_log_level_set("wifi", ESP_LOG_WARN);
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init_sta();

    // Wait until connected to wifi
    xEventGroupWaitBits(wifi_event_group,
                        WIFI_CONNECTED_BIT,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);

    ESP_LOGI(WIFI_TAG, "WiFi connected, starting HTTP request");

    xTaskCreate(uart_event_task, "uart_event_task",
                4096, NULL, 5, NULL);

    xTaskCreate(&http_get_task, "http_get_task", 8192, NULL, 5, NULL);
    // xTaskCreate(fake_uart_task, "fake_uart", 4096, NULL, 5, NULL);
}
