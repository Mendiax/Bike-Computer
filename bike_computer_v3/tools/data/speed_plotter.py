#!/bin/python3
import sys
import gmplot
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np

def plot_gears(file_name):
    file = pd.read_csv(file_name, delimiter=";")

    gear = file['gear']
    velocity = file['velocity_gpio']
    cadence = file['cadence']

    time_stamps = file['time']
    xAxis = [i for i in range(len(time_stamps))]

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    plt.plot(xAxis, gear, color=colors[0], marker='.', label="Wybrany bieg")
    plt.plot(xAxis, cadence, color=colors[1], marker='.', label="kadencja")
    plt.plot(xAxis, velocity, color=colors[2], marker='.', label="prędkość")



    plt.xlabel('czas [s]')
    plt.ylabel("wysokość [m]")
    plt.title("Porównanie prędkości z modułu GPS i z czujnika koła")

    plt.grid(True)
    plt.legend(loc='upper right')
    plt.savefig('gear_speed_cadence.png')
    plt.show()

def plot_const_gears(file_name, output = None):
    file = pd.read_csv(file_name, delimiter=";")

    gear = file['gear']
    velocity = file['velocity_gpio']
    cadence = file['cadence']

    time_stamps = file['time']
    xAxis = [i for i in range(len(time_stamps))]

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17


    fig, ax1 = plt.subplots(figsize=(9, 6))
    fig.subplots_adjust(right=0.75)
    # Instantiate a second axes that shares the same x-axis
    ax2 = ax1.twinx()
    ax3 = ax1.twinx()

    ax3.spines.right.set_position(("axes", 1.2))

    ax1.set_ylim(0, 12)
    ax2.set_ylim(0, 120);
    ax3.set_ylim(0, 30);


    p1, = ax1.plot(xAxis, gear, color=colors[0], marker='.', label="bieg")
    p2, = ax2.plot(xAxis, cadence, color=colors[1], marker='.', label="kadencja")
    p3, = ax3.plot(xAxis, velocity, color=colors[2], marker='.', label="prędkość")


    ax1.set_xlabel("Czas [s]")
    ax1.set_ylabel("bieg")
    ax2.set_ylabel("kadencja [obr/min]")
    ax3.set_ylabel("prędkość [km/h]")

    # plt.xlabel('czas [s]')
    # plt.ylabel("wysokość [m]")
    # plt.title("Przedstawienie jazdy na stałym biegu ze zmienną prędkością i kadencją")

    plt.grid(True)
    if output != "accel":
        plt.legend(loc='upper right', handles=[p1, p2, p3])
    else:
        plt.legend(loc='lower right', handles=[p1, p2, p3])

    filename = "gear_const"
    if output is not None:
        file_name = output

    plt.savefig(file_name + '.svg', format="svg")
    plt.show()

def plot_gps_vs_bar(file_name, output):
    file = pd.read_csv(file_name, delimiter=";")

    altitude_press = file['altitude_press']
    altitude_gps = file['altitude_gps']
    altitude_map = file['height']

    time_stamps = file['time']
    xAxis = [i for i in range(len(altitude_gps))]

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    altitude_press = altitude_press + 129.5

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(xAxis, altitude_press, color=colors[0], marker='.', label="wysokość na podstawie ciśnienia")
    ax.plot(xAxis, altitude_gps, color=colors[1], marker='.', label="wysokość pobrana z gps")
    ax.plot(xAxis, altitude_map, color=colors[2], marker='o', ls=' ', markersize=12, label="wysokość z mapy hipsometrycznej")


    ax.set_xlabel('czas [s]')
    ax.set_ylabel("wysokość [m]")
    # plt.title("Porównanie prędkości z modułu GPS i z czujnika koła")

    ax.grid(True)
    plt.legend(loc='upper right')

    filename = "height_gps_bar"
    if output is not None:
        file_name = output

    plt.savefig(file_name + '.svg', format="svg")
    plt.show()

def plot_gps_vs_gpio(file_name, output = None):
    file = pd.read_csv(file_name, delimiter=";")

    v_gps = file['velocity_gps']
    v_wheel = file['velocity_gpio']
    altitude = file['altitude_press']

    time_stamps = file['time']
    xAxis = [i for i in range(len(v_gps))]

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    # xAxis = time_stamps
    # 125 dla 30_13_29
    altitude_norm = [i + 125 for i in altitude]

    fig, ax1 = plt.subplots(figsize=(9, 6))
    # fig.subplots_adjust(right=0.75)
    # Instantiate a second axes that shares the same x-axis
    # ax2 = ax1.twinx()
    # ax3 = ax1.twinx()

    # ax3.spines.right.set_position(("axes", 1.2))

    ax1.set_ylim(0, 30);
    # ax2.set_ylim(100, 140)
    # ax3.set_ylim(0, 30);


    p2, = ax1.plot(xAxis, v_gps, color=colors[1], marker='.', label="prędkość gps")
    p3, = ax1.plot(xAxis, v_wheel, color=colors[2], marker='.', label="prędkość koło")
    # p1, = ax2.plot(xAxis, altitude_norm, color=colors[0], marker='.', label="wysokość")


    ax1.set_xlabel("Czas [s]")
    ax1.set_ylabel("prędkość [km/h]")
    # ax1.set_ylabel("wysokość")
    # ax2.set_ylabel("kadencja [obr/min]")

    # plt.plot(xAxis, altitude, color=colors[2], marker='.', label="altitude")
    # plt.plot(xAxis, v_gps, color=colors[0], marker='.', label="gps")
    # plt.plot(xAxis, v_wheel, color=colors[1], marker='.', label="wheel")

    # plt.xlabel('czas [s]')
    # plt.ylabel("prędkośc [kph]")
    # plt.title("Porównanie prędkości z modułu GPS i z czujnika koła")

    plt.grid(True)
    plt.legend(loc='upper right')
    # plt.savefig('gps_and_wss.png')

    filename = "gear_const"
    if output is not None:
        file_name = output

    plt.savefig(file_name + '.svg', format="svg")
    plt.show()



def plot_diff(file_name):
    file = pd.read_csv(file_name, delimiter=";")

    v_gps = file['velocity_gps']
    v_wheel = file['velocity_gpio']

    time_stamps = file['time']

    colors = ['blue', 'red', 'orange', 'purple', 'green', 'lime', 'cyan', 'black', "silver", "maroon", "violet", "yellow",
          "gold", "crimson", "navy", "forestgreen", '#ffff55']  # 17

    # xAxis = time_stamps
    dv = abs(v_wheel - v_gps)/v_wheel
    dv = dv * 100

    xAxis = [i for i in range(len(dv))]

    plt.plot(xAxis, dv, color=colors[1], marker='.', label="błąd bezwzględny")


    altitude = file['altitude_press']
    altitude_norm = [i/1 for i in altitude]
    altitude_delta = np.diff(altitude_norm)/0.1

    render = altitude_delta
    xAxis = [i for i in range(len(render))]
    plt.plot(xAxis, render, color=colors[2], marker='.', label="zmiana wysokości")

    plt.xlabel('czas [s]')
    plt.ylabel("błąd względny gps [%]")
    # plt.title("Porównanie błedu gps względem czujnika koła")
    plt.grid(True)
    plt.legend(loc='upper right')
    plt.savefig('gps_vs_wss.png')
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
    # file_name = get_sys_arg("file", None)
    # file_name = sys.argv[1]
    # plot_gears(file_name)
    # plot_gps_vs_gpio(file_name)
    # plot_diff(file_name)
    plot_gps_vs_bar(sys.argv[1], "h_gps_bar")

    # plot_const_gears("log/gps_log_accel_upshift.csv", "accel")
    # plot_const_gears("log/gps_log_const_speed.csv", "const_gear")




if __name__ == "__main__":
    main()