#!/bin/python3
import sys
import gmplot
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np


def val_in_range(val, range_low, range_high):
    return val >= range_low and val <= range_high

gear_up_shift = "upshift ^^"
gear_down_shift = "downshift .."
gear_no_shift = "no shift"

class Current_State:
    speed = None
    cadence = None
    gear = None

    gears = {
        "front" : [32],
        "rear" : [51, 45, 39, 33, 28, 24, 21, 18, 15, 13, 11]
    }

    gear_ratios = []
    gear_diff = 100.0

    def __init__(self):
        for f in self.gears["front"]:
            for r in self.gears["rear"]:
                self.gear_ratios.append(f/r)
        print(self.gear_ratios)

        self.gear_diff = 100.0
        last_ratio = 0.0
        for ratio in self.gear_ratios:
            self.gear_diff = min(abs(last_ratio - ratio), self.gear_diff)
            last_ratio = self.gear_diff
        self.gear_diff = self.gear_diff * 0.5
        print(self.gear_diff)

    def get_current_ratio(self):
        return self.gear_ratios[self.gear - 1]




def plot(file_name):
    file = pd.read_csv(file_name, delimiter=";")

    gears_idx = file['gear']
    ratio = file['read_ratio']

    bike = Current_State()

    gear_range = []

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    xAxis = file.index

    plt.plot(xAxis, gears_idx, color=colors[0], marker='.', label="gear")
    plt.plot(xAxis, ratio, color=colors[1], marker='.', label="ratio")

    index = 2
    for gear in bike.gear_ratios:
        ratio_min = gear - bike.gear_diff
        ratio_max = gear + bike.gear_diff
        plt.plot(xAxis, [ratio_min] * len(xAxis), color=colors[index], marker='.')
        plt.plot(xAxis, [ratio_max] * len(xAxis), color=colors[index], marker='.')
        index += 1




    plt.xlabel('time')
    plt.ylabel("gear/ratio")

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