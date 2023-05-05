import serial
import re
import matplotlib.pyplot as plt
import threading
import glob
import time
import os
import subprocess
import sys

class PicoDevice(serial.Serial):

    def get_devices(self):
        devices = []

        try_count = 0
        while len(devices) == 0 and try_count <= 5:
            devices = glob.glob('/dev/ttyACM*')
            print(f'[{try_count}]Avaible devices:', devices)
            try_count += 1
            if len(devices) == 0:
                time.sleep(1)
        return devices

    def __init__(self) -> None:

        try_count = 0
        devices = self.get_devices()
        while len(devices) == 0 and try_count <= 3:
            self.reset()
            try_count += 1
            devices = self.get_devices()



        # TODO add choosing device
        # now is the first one
        pico_dev = devices[0]
        print('Picked:')
        print(pico_dev)
        print(subprocess.getoutput("pwd"))
        # print(subprocess.getoutput(f'stat /dev/'))
        # print(subprocess.getoutput(f'stat {pico_dev}'))
        super().__init__(pico_dev)

    def get_line(self) -> str:
        return self.readline().decode('utf-8').strip()

    def reset(self) -> None:
        print(subprocess.getoutput("picotool reboot -f"))
        time.sleep(5.0)
        self.__init__()

# Serial port configuration

# Plot configuration
plot_interval = 0.1  # interval between plot updates, in seconds
plot_window = 10  # size of the plot window, in seconds
plots = {}  # dictionary to store plot data

# Regular expression to parse input lines
pattern = re.compile(r'.*\[PLOT\s+<([a-zA-Z\s]+)>\]\s([\d.]+).*')


# Function to read and process serial input
def read_serial(ser):
    while True:
        line = ser.readline().decode().strip()
        match = pattern.match(line)
        if match:
            name = match.group(1)
            value = float(match.group(2))
            if name not in plots:
                plots[name] = {'times': [], 'values': []}
            plots[name]['times'].append(time.time())
            plots[name]['values'].append(value)
        else:
            print(f"{line}")

# Function to remove old data from plot
def remove_data(name):
    plots[name]['times'].pop(0)
    plots[name]['values'].pop(0)

# Function to update plot
def update_plot():
    def on_close(event):
        sys.exit(0)

    fig = plt.figure()
    fig.canvas.mpl_connect('close_event', on_close)

    while True:
        plt.clf()
        plt.xlabel('Time (s)')
        plt.ylabel('Value')
        # plt.ylim(0, 100024)
        for name, data in plots.items():
            # times = [t for t in data['times']]
            times = [i for i in range(len(data['values']))]
            plt.plot(data['times'], data['values'], label=name)
        plt.legend()
        plt.pause(plot_interval)

# Main program
if __name__ == '__main__':
    # Open serial port
    ser = PicoDevice()

    # Start reading and processing serial input in a separate thread
    serial_thread = threading.Thread(target=read_serial, args=[ser], daemon=True)
    serial_thread.start()

    # Start updating plot in a separate thread
    # plot_thread = threading.Thread(target=update_plot, daemon=True)
    # plot_thread.start()
    update_plot()

    # time.sleep(5)

    # Show the plot
    plt.show()

    # Close the serial port when the plot window is closed
    ser.close()
