#!/bin/python3
import sys
import gmplot
import pandas as pd




def get_data(file_name):
    file = pd.read_csv(file_name, delimiter=";")
    latitude_list = file['latitude'].to_list()
    longitude_list= file['longitude'].to_list()
    # latitude_list = [ 30.3358376, 30.307977, 30.3216419 ]
    # longitude_list = [ 77.8701919, 78.048457, 78.0413095 ]
    print("latitude_list:  ", latitude_list)
    print("longitude_list: ", longitude_list)

    return latitude_list, longitude_list





def plot(latitude_list, longitude_list):
    gmap3 = gmplot.GoogleMapPlotter(latitude_list[0],
                                    longitude_list[0], 10)

    # scatter method of map object
    # scatter points on the google map
    gmap3.scatter( latitude_list, longitude_list, '#FF0000',
                                size = 10, marker = False )

    # Plot method Draw a line in
    # between given coordinates
    gmap3.plot(latitude_list, longitude_list,
            'cornflowerblue', edge_width = 2.5)

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
    lat, long = get_data(file_name)
    plot(lat, long)

if __name__ == "__main__":
    main()