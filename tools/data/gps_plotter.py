#!/bin/python3
import sys
import gmplot
import pandas as pd




def get_data(file_name):
    file = pd.read_csv(file_name, delimiter=";")
    latitude_list = file['latitude'].to_list()
    longitude_list= file['longitude'].to_list()
    idx = file['idx'].to_list()

    v_gps = file['velocity_gps']
    v_wheel = file['velocity_gpio']

    dv = v_wheel - v_gps
    dv_max = max(dv)
    dv_min = min(dv)

    dv_0_1 = [ max(((v - dv_min)/(dv_max - dv_min)), 0) for v in dv]
    # print(max(dv_0_1))


    # colors = ["#" + str(hex(int(0xff * v) * (2**16)))[2:] for v in dv_0_1]
    colors = ["#{0:02x}{0:02x}{0:02x}".format(int(0xff * v)) for v in dv_0_1]

    # print(colors)
    # latitude_list = [ 30.3358376, 30.307977, 30.3216419 ]
    # longitude_list = [ 77.8701919, 78.048457, 78.0413095 ]
    # print("latitude_list:  ", latitude_list)
    # print("longitude_list: ", longitude_list)

    return latitude_list, longitude_list, colors, idx


def insert_idx(file_name):
    file = pd.read_csv(file_name, delimiter=";")
    new_data = file.drop_duplicates(subset=['latitude', 'longitude'], inplace=False)
    new_data.to_csv("hill_index.csv",sep=";")


def deg_to_dms(deg, pretty_print=None, ndp=4):
    """Convert from decimal degrees to degrees, minutes, seconds."""

    m, s = divmod(abs(deg)*3600, 60)
    d, m = divmod(m, 60)
    if deg < 0:
        d = -d
    d, m = int(d), int(m)

    if pretty_print:
        if pretty_print=='latitude':
            hemi = 'N' if d>=0 else 'S'
        elif pretty_print=='longitude':
            hemi = 'E' if d>=0 else 'W'
        else:
            hemi = '?'
        return '{d:d}° {m:d}′ {s:.{ndp:d}f}″ {hemi:1s}'.format(
                    d=abs(d), m=m, s=s, hemi=hemi, ndp=ndp)
    return d, m, s

def plot(latitude_list, longitude_list, colors, idx):
    gmap3 = gmplot.GoogleMapPlotter(latitude_list[0],
                                    longitude_list[0], 14)


    labels=[]

    for (l,j,i) in zip(latitude_list,longitude_list,idx):
        label = "[" + str(i) + "] " + str(deg_to_dms(l,pretty_print='latitude',ndp=2)) + "|" + str(deg_to_dms(j,pretty_print='longitude', ndp=2))
        labels.append(label)

    # scatter method of map object
    # scatter points on the google map
    gmap3.scatter( latitude_list, longitude_list, '#FFFFFF',
                                size = 1, marker = True, alpha=1.0, label=labels)

    # Plot method Draw a line in
    # between given coordinates
    gmap3.plot(latitude_list, longitude_list,
            'cornflowerblue', edge_width = 1)

    # Pass the absolute path
    gmap3.draw( "/mnt/c/__DEV/Bike_computer/bike_computer_v3/data/map.html" )


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
    file_name = get_sys_arg("file", "log/gps_log_2022_10_09_20_51.csv")
    # insert_idx(file_name)
    lat, long, colors, idx = get_data(file_name)
    plot(lat, long, colors, idx)

if __name__ == "__main__":
    main()