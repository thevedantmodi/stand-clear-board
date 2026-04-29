#!/usr/bin/env python3
"""Send test 12-byte arrival packets to the STM32 over USART2 (USB VCP)."""

import serial
import time
import sys

# Find port: ls /dev/tty.* and look for usbmodem or usbserial
PORT = "/dev/cu.usbmodem2103"  # update this
BAUD = 9600

# SubwayId_T enum values
SUB_ONE, SUB_TWO, SUB_THREE = 0, 1, 2
SUB_FOUR, SUB_FIVE, SUB_SIX = 3, 4, 5
SUB_SEVEN = 6
SUB_A, SUB_B, SUB_C, SUB_D = 7, 8, 9, 10
SUB_E, SUB_F, SUB_G, SUB_H = 11, 12, 13, 14
SUB_J, SUB_L, SUB_M, SUB_N = 15, 16, 17, 18
SUB_Q, SUB_R, SUB_W, SUB_Z = 19, 20, 21, 22

UPTOWN   = 0
DOWNTOWN = 1
INVALID  = 0xFF  # sentinel — slot not shown

def make_packet(arrivals):
    """arrivals: list of (minutes, line_id, direction) tuples, length 4.
    Use INVALID as minutes to mark a slot as empty."""
    assert len(arrivals) == 4
    packet = bytes()
    for minutes, line_id, direction in arrivals:
        if minutes == INVALID:
            packet += bytes([INVALID, 0, 0])
        else:
            packet += bytes([minutes, line_id, direction])
    return packet


if __name__ == "__main__":
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"Opened {PORT} at {BAUD} baud")

    # Test packet: 4 real arrivals
    test1 = make_packet([
        (3,  SUB_A, DOWNTOWN),
        (7,  SUB_C, UPTOWN),
        (12, SUB_ONE, DOWNTOWN),
        (2,  SUB_L, UPTOWN),
    ])

    # Test packet: 2 arrivals, 2 empty slots
    test2 = make_packet([
        (5,      SUB_F, DOWNTOWN),
        (INVALID, 0,    0),
        (INVALID, 0,    0),
        (11,     SUB_N, UPTOWN),
    ])

    # Test packet: all empty (should blank the display)
    test_blank = make_packet([
        (INVALID, 0, 0),
        (INVALID, 0, 0),
        (INVALID, 0, 0),
        (INVALID, 0, 0),
    ])

    packets = [("4 arrivals", test1), ("2 arrivals", test2), ("blank", test_blank)]

    for name, pkt in packets:
        print(f"Sending {name}: {list(pkt)}")
        ser.write(pkt)
        time.sleep(2)

        # Print any debug output from printf on USART2
        while ser.in_waiting:
            print("STM32:", ser.readline().decode(errors="replace").strip())

    ser.close()
