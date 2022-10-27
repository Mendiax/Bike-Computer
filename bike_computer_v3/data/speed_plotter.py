#!/bin/python3
import sys
import gmplot
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np



def plot(file_name):
    file = pd.read_csv(file_name, delimiter=";")

    v_gps = file['velocity_gps']
    v_wheel = file['velocity_gpio']

    time_stamps = file['time']



    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    # xAxis = time_stamps

    # plt.plot(xAxis, v_gps, color=colors[0], marker='.', label="gps")
    # plt.plot(xAxis, v_wheel, color=colors[1], marker='.', label="wheel")

    dv = v_wheel - v_gps
    dv = [v for v in dv if abs(v) < 10]

    dv_avg = sum(dv)/len(dv)
    print(dv_avg/3.6)
    dv = [v - dv_avg for v in dv]

    xAxis = [i for i in range(len(dv))]


    plt.plot(xAxis, dv, color=colors[1], marker='.', label="dv")



    plt.xlabel('time')
    plt.ylabel("speed [kph]")

    plt.grid(True)
    plt.legend(loc='upper right')
    plt.savefig('gear.png')
    plt.show()



# args
def get_arg(args_list, param, default=None):
    prefix = str(param) + "="
    for arg in args_list:
        if str(arg).startswith(prefix):
            return arg[len(prefix):]
    return default

def get_sys_arg(param, default=None):
    return get_arg(sys.argv, param, default)

def main():
    file_name = get_sys_arg("file", None)
    plot(file_name)

if __name__ == "__main__":
    main()