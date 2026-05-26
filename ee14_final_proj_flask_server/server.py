from flask import Flask, make_response, request
from train_info import lines, line_index_mapping, stations, station_plus_line_to_ID_mapping
import json
import requests

app = Flask(__name__)

@app.route("/")
def http_get_default():
    response = requests.get('https://standclear.vedantmodi.com/api/arrivals/?line=6&stop_id=640N')
    if response.status_code == 200:
        data = response.json()
        fields = ['time_to_arrival', 'line', 'direction']
        modified_data = [[train_info[field] for field in fields] for train_info in data]
        print(modified_data)
        byte_list = []
        for train in modified_data:
            train[0] = round(int(train[0]) / 60)
            train[1] = line_index_mapping[train[1]]
            train[2] = int(train[2])
            byte_list.append(bytes(train))


        # print(modified_data)

        byte_list = byte_list[:4]
        while len(byte_list) < 4:
            byte_list.append(b'\xff\xff\xff')

        comb_bytes = b"".join(byte_list)

        # print(comb_bytes)
        # print(len(comb_bytes))

        return make_response(comb_bytes)
    
    return "Error"

@app.route("/arrivals")
def http_get():
    station_i = request.args.get('station', type=int)
    lines = request.args.getlist('line')

    if not station_i or not lines:
        print('bad query params')
        return "Error" #TODO
    
    #TODO check if lines are sentinel NULL values (0?)
    lines = [line for line in lines if line != '0']

    print(stations[station_i])
    print(lines)

    station = stations[station_i]

    pairs_to_query = []
    for line in lines:
        stationID = station_plus_line_to_ID_mapping.get((station, line))
        if stationID:
            pairs_to_query.append((line, f'{stationID}N'))
            pairs_to_query.append((line, f'{stationID}S'))

    train_list = []
    for line, id in pairs_to_query:
        response = requests.get(f'https://standclear.vedantmodi.com/api/arrivals/?line={line}&stop_id={id}')
        if response.status_code == 200:
            data = response.json()
            fields = ['time_to_arrival', 'line', 'direction']
            for train_info in data:
                train_list.append([train_info[field] for field in fields])
        else:
            print('Error', line, id)

    train_list.sort(key=lambda train: train[0]) # sort by time of arrival
    train_list = train_list[:4]
    print(train_list)

    byte_list = []
    for train in train_list:
        train[0] = min(round(int(train[0]) / 60), 10) # Convert to minutes, capped at 10
        train[1] = line_index_mapping[train[1]]
        train[2] = int(train[2])
        byte_list.append(bytes(train))
    
    while len(byte_list) < 4:
        byte_list.append(b'\xff\xff\xff')

    comb_bytes = b''.join(byte_list)
    print(comb_bytes)
    return make_response(comb_bytes)

if __name__ == "__main__":
    app.run(debug=True)

