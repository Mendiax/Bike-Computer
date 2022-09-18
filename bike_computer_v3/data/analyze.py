#!/bin/python3

from cProfile import label
import sys
from typing_extensions import Self
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np

class GearData:
    def __init__(self, data : str) -> None:
        data_arr = data.split(",")

        # convert to int
        data_params = data_arr[0][1:-1].split("|")
        data_params = np.asarray(data_params, dtype=int)

        data_arr = data_arr[1:]
        data_arr = [float(x) for x in data_arr] #data_arr.astype(float)#np.asarray(data_arr, dtype=float)

        # print(data_params)
        # print(data_arr)

        self.no_gears, self.cadence_min, self.cadence_max, self.cadence_delta = data_params
        # print(self.no_gears)
        # print(self.cadence_min)
        # print(self.cadence_max)
        # print(self.cadence_delta)
        data_len = int((self.cadence_max - self.cadence_min) / self.cadence_delta) + 2
        # print(data_len)
        self.cadence_gears = []
        for gear in range(self.no_gears):
            data_start = gear * data_len
            data_end = (gear + 1) * data_len
            self.cadence_gears.append(data_arr[data_start:data_end])
        # print(self.cadence_gears)

    def get_gear_at(self, gear : int):
        if gear == 0 or gear > self.no_gears:
            raise Exception("wring gear %d" % gear)
        return self.cadence_gears[gear - 1]

    def get_gear_all(self):
        return self.cadence_gears

    def get_cadence_labels(self):
        min_label = "<" + str(self.cadence_min)
        max_label = ">" + str(self.cadence_max)
        labels = [min_label]
        for x in range(self.cadence_min, self.cadence_max, self.cadence_delta):
            labels.append(str(x) + "-" + str(x + self.cadence_delta))
        labels.append(max_label)
        return labels



def analyze_db(file_name : str, id : str):
    print(f"loading record from {file_name} at {id}")
    file = pd.read_csv(file_name, delimiter=";")

    gears = file['gears'][int(id)]
    # print(gears)
    data_analyze = GearData(gears)

    labels = data_analyze.get_cadence_labels()
    # print(labels)

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    xAxis = labels

    plt.title('Gear time(cadence)')
    index = 1
    for gear_data in data_analyze.get_gear_all():
        label = f"gear{index}"
        yAxis = gear_data
        plt.plot(xAxis, yAxis, color=colors[index], marker='.', label=label)
        index += 1
    plt.xlabel('cadence [rpm]')
    plt.ylabel("time [s]")
    plt.grid(True)
    plt.legend(loc='upper right')
    plt.show()

def print_db(file_name : str):
    print(f"loading instructions {file_name}")
    file = pd.read_csv(file_name, delimiter=";")
    print(file)



# args
def get_arg(args_list, param, default=None):
    prefix = str(param) + "="
    # print(prefix)
    # assert str("file=x").startswith("file=") == True
    for arg in args_list:
        if str(arg).startswith(prefix):
            return arg[len(prefix):]
    return default

def get_sys_arg(param, default=None):
    return get_arg(sys.argv, param, default)

def main():
    file_name = get_sys_arg("file", "log/last_track.csv")
    analyze = get_sys_arg("analyze")
    if analyze is not None:
        analyze_db(file_name, analyze)
    else:
        print_db(file_name)

if __name__ == "__main__":
    main()
