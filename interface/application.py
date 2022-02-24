import os
import sys

import serial
import serial.tools.list_ports
import threading
import time
from datetime import datetime
import tkinter as tk
import tkinter.ttk as ttk
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import configparser

matplotlib.use("TkAgg")
plt.tight_layout()

config = configparser.ConfigParser()
config.read('config.ini')

data_path = config['default']['readings_folder']
tmp116_data = data_path + config['default']['tmp116_readings']
hdc2010_temperature_data = data_path + config['default']['hdc2010_temperature_readings']
hdc2010_humidity_data = data_path + config['default']['hdc2010_humidity_readings']
opt3001_data = data_path + config['default']['opt3001_readings']
dps310_temperature_data = data_path + config['default']['dps310_temperature_readings']
dps310_pressure_data = data_path + config['default']['dps310_pressure_readings']

csv_files = [tmp116_data,
             hdc2010_temperature_data,
             hdc2010_humidity_data,
             opt3001_data,
             dps310_temperature_data,
             dps310_pressure_data]

column_names = ['Vrijeme', 'Senzor', 'Velicina', 'Iznos']

BUFFER_LENGTH = config['default']['buffer_window']

serial_ports = serial.tools.list_ports.comports()
serial_port = config['default']['port']
baud_rate = config['default']['baud']
serial_status = False

baud_rates = [110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000]

LARGE_FONT = ("Verdana", 18)
MEDIUM_FONT = ("Verdana", 14)
SMALL_FONT = ("Verdana", 12)
APP_NAME = 'Pametni stan'
TEMPERATURE_INTERVAL = 2500
HUMIDITY_INTERVAL = 2500
LIGHT_INTERVAL = 500
PRESSURE_INTERVAL = 500

WINDOW_X = 1400
WINDOW_Y = 800
FIGURE_SIZE = (7, 5)
DPI = 60

measurement_lut = {
    'T': ["Temperatura zraka", "°C", [0.07, 0.2125], [0.36, 0.2125]],
    'H': ["Vlažnost zraka", "%", [0.07, 0.25], [0.36, 0.25]],
    'P': ["Atmosferski tlak", "hPa", [0.07, 0.375], [0.36, 0.375]],
    'L': ["Ambijentalna svjetlost", "lux", [0.07, 0.4125], [0.36, 0.4125]]
}

TEMP_COMFORT_LOW = float(config['default']['temperature_comfort_low'])
TEMP_COMFORT_HIGH = float(config['default']['temperature_comfort_high'])
HUM_COMFORT_LOW = int(config['default']['humidity_comfort_low'])
HUM_COMFORT_HIGH = int(config['default']['humidity_comfort_high'])
LIGHT_THRESHOLD = int(config['default']['light_threshold'])
TEMP_MIN = float(config['default']['temperature_low'])
TEMP_MAX = float(config['default']['temperature_high'])
HUM_MIN = int(config['default']['humidity_low'])
HUM_MAX = int(config['default']['humidity_high'])
LUX_MIN = int(config['default']['light_min'])
LUX_MAX = int(config['default']['light_max'])
PRESSURE_JUMP = int(config['default']['pressure_jump_threshold'])
PRESSURE_MIN = 700
PRESSURE_MAX = 1100

light_status = 0
heating_status = 0
cooling_status = 0
humidifier_status = 0

open_timestamp = ""


def file_check():
    if not os.path.exists(data_path):
        os.makedirs(data_path)

    if len(os.listdir(data_path)) == 0:
        open(tmp116_data, 'a').close()
        open(hdc2010_temperature_data, 'a').close()
        open(hdc2010_humidity_data, 'a').close()
        open(opt3001_data, 'a').close()
        open(dps310_temperature_data, 'a').close()
        open(dps310_pressure_data, 'a').close()


def check_serial():
    global serial_status
    for port in serial_ports:
        if serial_ports in port:
            serial_status = True
            print("dingus")
            return
    serial_status = False
    print("dongus")


def read_serial():
    with open(tmp116_data, 'a', newline='') as tmp116_file, \
            open(hdc2010_temperature_data, 'a', newline='') as hdc2010_temp_file, \
            open(hdc2010_humidity_data, 'a', newline='') as hdc2010_hum_file, \
            open(opt3001_data, 'a', newline='') as opt3001_file, \
            open(dps310_temperature_data, 'a', newline='') as dps310_temp_file, \
            open(dps310_pressure_data, 'a', newline='') as dps310_pressure_file:

        for i in range(len(csv_files)):
            serial_input = serial_connection.readline()

            timestamp = datetime.now().strftime("%d/%m/%Y %H:%M:%S")

            if serial_input:
                line = serial_input.decode()
                line_split = line.split(', ')

                if line_split[0] == 'TMP116':
                    tmp116_file.write(timestamp + ', ' + line)
                elif line_split[0] == 'HDC2010':
                    if line_split[1] == 'T':
                        hdc2010_temp_file.write(timestamp + ', ' + line)
                    elif line_split[1] == 'H':
                        hdc2010_hum_file.write(timestamp + ', ' + line)
                elif line_split[0] == 'OPT3001':
                    opt3001_file.write(timestamp + ', ' + line)
                elif line_split[0] == 'DPS310':
                    if line_split[1] == 'T':
                        dps310_temp_file.write(timestamp + ', ' + line)
                    elif line_split[1] == 'P':
                        dps310_pressure_file.write(timestamp + ', ' + line)


def write_serial():
    serial_connection.write(f"{light_status}{heating_status}{cooling_status}{humidifier_status}|"
                            .encode('utf-8'))


def clean_buffer(filelist):
    now = datetime.now()
    for file in filelist:
        with open(file, 'r') as buffer:
            valid = []
            lines = buffer.readlines()
            if len(lines) > 0:
                for row in lines:
                    timestamp = datetime.strptime(row.split(", ")[0], "%d/%m/%Y %H:%M:%S")
                    time_diff = now - timestamp
                    if int(BUFFER_LENGTH) * 60 >= time_diff.total_seconds() >= 0:
                        valid.append(row)
        buffer.close()

        with open(file, 'w') as buffer:
            buffer.writelines(valid)
        buffer.close()


def update_config():
    with open('config.ini', 'w') as configfile:
        config.write(configfile)


def check_pressure():
    global open_timestamp
    now = datetime.now()

    data = pd.read_csv(dps310_pressure_data, names=column_names)
    data['Iznos'] = pd.to_numeric(data['Iznos'], errors='coerce')
    data = data.dropna(subset=['Iznos'])

    recent = []

    if len(data) > 0:
        timestamps = data['Vrijeme'].values
        readings = data['Iznos'].values

        for i in range(len(timestamps)):
            timestamp = datetime.strptime(timestamps[i], "%d/%m/%Y %H:%M:%S")
            time_diff = now - timestamp
            if 2 >= time_diff.total_seconds() >= 0:
                recent.append(float(readings[i]))

    for i in range(1, len(recent)):
        if abs(recent[i] - recent[i - 1]) > PRESSURE_JUMP:
            open_timestamp = now.strftime("%H:%M")


figureTMP116 = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)
figureHDC2010TMP = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)
figureDPS310TMP = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)
figurePressure = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)
figureOPT = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)
figureHumidity = plt.Figure(figsize=FIGURE_SIZE, dpi=DPI)


def animateOPT(i):
    data = pd.read_csv(opt3001_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1], 2))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figureOPT.clear()
    axOPT = figureOPT.add_subplot(111)
    axOPT.plot(values, color='red')
    axOPT.set_title("OPT3001 - ambijentalno osvjetljenje")
    axOPT.set_ylabel("lux")
    axOPT.set_xlabel("Vrijeme")
    axOPT.axes.xaxis.set_ticks([])
    figureOPT.canvas.draw()


def animatePressure(i):
    data = pd.read_csv(dps310_pressure_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1] / 100, 4))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figurePressure.clear()
    axPressure = figurePressure.add_subplot(111)
    axPressure.plot(values, color='red')
    axPressure.set_title("DPS310 - atmosferski tlak")
    axPressure.set_xlabel("Vrijeme")
    axPressure.set_ylabel("hPa")
    axPressure.get_yaxis().get_major_formatter().set_useOffset(False)
    axPressure.axes.xaxis.set_ticks([])
    figurePressure.canvas.draw()


def animateTMP116(i):
    data = pd.read_csv(tmp116_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1], 2))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figureTMP116.clear()
    axTMP116 = figureTMP116.add_subplot(111)
    axTMP116.plot(values, color='red')
    axTMP116.set_title("TMP116")
    axTMP116.set_ylabel("°C")
    axTMP116.set_xlabel("Vrijeme")
    axTMP116.axes.xaxis.set_ticks([])
    figureTMP116.canvas.draw()


def animateHDC2010TMP(i):
    data = pd.read_csv(hdc2010_temperature_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1], 2))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figureHDC2010TMP.clear()
    axHDC2010TMP = figureHDC2010TMP.add_subplot(111)
    axHDC2010TMP.plot(values, color='red')
    axHDC2010TMP.set_title("HDC2010 - temperatura zraka")
    axHDC2010TMP.set_ylabel("°C")
    axHDC2010TMP.set_xlabel("Vrijeme")
    axHDC2010TMP.axes.xaxis.set_ticks([])
    figureHDC2010TMP.canvas.draw()


def animateDPS310TMP(i):
    data = pd.read_csv(dps310_temperature_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1], 2))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figureDPS310TMP.clear()
    axDPS310TMP = figureDPS310TMP.add_subplot(111)
    axDPS310TMP.plot(values, color='red')
    axDPS310TMP.set_title("DPS310 - temperatura zraka")
    axDPS310TMP.set_ylabel("°C")
    axDPS310TMP.set_xlabel("Vrijeme")
    axDPS310TMP.axes.xaxis.set_ticks([])
    figureDPS310TMP.canvas.draw()


def animateHumidity(i):
    data = pd.read_csv(hdc2010_humidity_data, names=column_names).values
    values = []
    timestamps = []

    for row in data:
        values.append(round(row[-1], 2))
        timestamp = datetime.strptime(row[0], "%d/%m/%Y %H:%M:%S")
        timestamps.append(timestamp)

    figureHumidity.clear()
    axHumidity = figureHumidity.add_subplot(111)
    axHumidity.plot(values, color='red')
    axHumidity.set_title("HDC2010 - relativna vlažnost zraka")
    axHumidity.set_ylabel("%")
    axHumidity.set_xlabel("Vrijeme")
    axHumidity.axes.xaxis.set_ticks([])
    figureHumidity.canvas.draw()


class MainView(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Pregled", font=LARGE_FONT)
        label.pack(padx=20, pady=30)

        avg_title = tk.Label(self, text="Prosječne vrijednosti", font=LARGE_FONT)
        avg_title.place(relx=0.07, rely=0.1625)

        current_title = tk.Label(self, text="Trenutne vrijednosti", font=LARGE_FONT)
        current_title.place(relx=0.07, rely=0.325)

        button = tk.Button(self, text="Očitanja senzora", command=lambda: controller.show_frame(GraphView))
        button.place(relx=0.07, rely=0.47)

        settings_title = tk.Label(self, text="Postavke", font=LARGE_FONT)
        settings_title.place(relx=0.7, rely=0.1625)

        port_label = tk.Label(self, text="Uređaj:", font=MEDIUM_FONT)
        port_label.place(relx=0.7, rely=0.225)
        self.port_box = ttk.Combobox(self, values=serial_ports)
        self.port_box.set(serial_port)
        self.port_box['state'] = 'readonly'
        self.port_box.place(relx=0.775, rely=0.230)

        baud_label = tk.Label(self, text="Baud:", font=MEDIUM_FONT)
        baud_label.place(relx=0.7, rely=0.2625)
        self.baud_box = ttk.Combobox(self, values=baud_rates)
        self.baud_box.set(baud_rate)
        self.baud_box['state'] = 'readonly'
        self.baud_box.place(relx=0.775, rely=0.270)

        button = tk.Button(self, text="Spremi", command=self.update_serial)
        button.place(relx=0.7, rely=0.3125)

        light_label = tk.Label(self, text="Razina svjetlosti pri kojoj se pali rasvjeta", font=SMALL_FONT)
        light_label.place(relx=0.07, rely=0.58)
        self.light_slider = tk.Scale(self, from_=LUX_MIN, to=LUX_MAX, sliderlength=20, length=250, orient=tk.HORIZONTAL)
        self.light_slider.set(LIGHT_THRESHOLD)
        self.light_slider.place(relx=0.07, rely=0.64)

        light_button = tk.Button(self, text="Ažuriraj", command=self.update_light)
        light_button.place(relx=0.07, rely=0.72)

        temp_label = tk.Label(self, text="Ugodni raspon temperature", font=SMALL_FONT)
        temp_label.place(relx=0.36, rely=0.58)
        self.temp_slider_lo = tk.Scale(self, from_=TEMP_MIN, to=TEMP_MAX,
                                  sliderlength=20, length=250, resolution=0.1,
                                  label="Donja granica", orient=tk.HORIZONTAL)

        self.temp_slider_lo.set(TEMP_COMFORT_LOW)
        self.temp_slider_lo.place(relx=0.36, rely=0.625)

        self.temp_slider_hi = tk.Scale(self, from_=TEMP_MIN, to=TEMP_MAX,
                                  sliderlength=20, length=250, resolution=0.1,
                                  label="Gornja granica", orient=tk.HORIZONTAL)
        self.temp_slider_hi.set(TEMP_COMFORT_HIGH)
        self.temp_slider_hi.place(relx=0.36, rely=0.72)

        temp_button = tk.Button(self, text="Ažuriraj", command=self.update_temp)
        temp_button.place(relx=0.36, rely=0.82)

        self.temp_status = tk.Label(self, text="", font=SMALL_FONT)
        self.temp_status.place(relx=0.36, rely=0.8325)

        hum_label = tk.Label(self, text="Ugodni raspon vlažnosti zraka", font=SMALL_FONT)
        hum_label.place(relx=0.64, rely=0.58)
        self.hum_slider_lo = tk.Scale(self, from_=HUM_MIN, to=HUM_MAX,
                                  sliderlength=20, length=250,
                                  label="Donja granica", orient=tk.HORIZONTAL)

        self.hum_slider_lo.set(HUM_COMFORT_LOW)
        self.hum_slider_lo.place(relx=0.64, rely=0.625)

        self.hum_slider_hi = tk.Scale(self, from_=HUM_MIN, to=HUM_MAX,
                                  sliderlength=20, length=250,
                                  label="Gornja granica", orient=tk.HORIZONTAL)
        self.hum_slider_hi.set(HUM_COMFORT_HIGH)
        self.hum_slider_hi.place(relx=0.64, rely=0.72)

        hum_button = tk.Button(self, text="Ažuriraj", command=self.update_hum)
        hum_button.place(relx=0.64, rely=0.82)

        self.hum_status = tk.Label(self, text="", font=SMALL_FONT)
        self.hum_status.place(relx=0.64, rely=0.8325)

        self.readings = {}
        self.update_data()

    def update_data(self):
        long_term_files = [tmp116_data, hdc2010_humidity_data]
        short_term_files = [dps310_pressure_data, opt3001_data]

        for file in long_term_files:
            data = pd.read_csv(file, names=column_names)
            if not data.empty:
                unit = data['Velicina'][0].strip()
                data['Iznos'] = pd.to_numeric(data['Iznos'], errors='coerce')
                data = data.dropna(subset=['Iznos'])

                if unit == 'T':
                    average = round(data['Iznos'].mean(), 1)
                else:
                    average = round(data['Iznos'].mean())

                avg_message = f"-{measurement_lut[unit][0]}: " + str(average) + ' ' + measurement_lut[unit][1]
                avg_label = tk.Label(self, text=avg_message, font=MEDIUM_FONT)
                avg_label.place(relx=measurement_lut[unit][2][0], rely=measurement_lut[unit][2][1])
                avg_label.after(1250, avg_label.destroy)
                self.readings[unit] = average

        for file in short_term_files:
            data = pd.read_csv(file, names=column_names)
            if not data.empty:
                unit = data['Velicina'][0].strip()
                data['Iznos'] = pd.to_numeric(data['Iznos'], errors='coerce')
                data = data.dropna(subset=['Iznos'])
                measurements = data['Iznos'].values
                current = measurements[-1]

                if unit == 'P':
                    current = round(current / 100, 1)
                else:
                    current = round(current)

                current_message = f"-{measurement_lut[unit][0]}: " + str(current) + ' ' + measurement_lut[unit][1]
                current_label = tk.Label(self, text=current_message, font=MEDIUM_FONT)
                current_label.place(relx=measurement_lut[unit][2][0], rely=measurement_lut[unit][2][1])
                current_label.after(1250, current_label.destroy)
                self.readings[unit] = current

    def check_actions(self):
        global light_status, humidifier_status, heating_status, cooling_status

        if serial_status:
            if not serial_connection.isOpen():
                serial_error_label = tk.Label(self, text="Uređaj nije spojen")
                serial_error_label.place(x=1000, y=310)
        else:
            serial_error_label = tk.Label(self, text="Uređaj nije pronađen")
            serial_error_label.place(x=1000, y=310)

        for unit in self.readings:
            value = self.readings[unit]
            message_text = ""
            if unit == 'T':
                if TEMP_COMFORT_HIGH > value > TEMP_COMFORT_LOW:
                    message_text = "Temperatura je ugodna"
                    heating_status = 0
                    cooling_status = 0
                elif value >= TEMP_COMFORT_HIGH:
                    message_text = "Temperatura je visoka, upalite klimu"
                    heating_status = 0
                    cooling_status = 1
                elif value <= TEMP_COMFORT_LOW:
                    message_text = "Temperatura je niska, upalite grijanje"
                    heating_status = 1
                    cooling_status = 0

                message_label = tk.Label(self, text=message_text, font=MEDIUM_FONT)
                message_label.place(relx=measurement_lut[unit][3][0], rely=measurement_lut[unit][3][1])
                message_label.after(1250, message_label.destroy)

            elif unit == 'H':
                if HUM_COMFORT_HIGH > value > HUM_COMFORT_LOW:
                    message_text = "Vlažnost zraka je ugodna"
                    humidifier_status = 0
                elif value >= HUM_COMFORT_HIGH:
                    message_text = "Vlažnost zraka je visoka"
                    humidifier_status = 0
                elif value <= HUM_COMFORT_LOW:
                    message_text = "Vlažnost zraka je niska"
                    humidifier_status = 1

                message_label = tk.Label(self, text=message_text, font=MEDIUM_FONT)
                message_label.place(relx=measurement_lut[unit][3][0], rely=measurement_lut[unit][3][1])
                message_label.after(1250, message_label.destroy)

            elif unit == 'L':
                if value <= LIGHT_THRESHOLD:
                    message_text = "Mračno je, upalite svjetlo"
                    message_label = tk.Label(self, text=message_text, font=MEDIUM_FONT)
                    message_label.place(relx=measurement_lut[unit][3][0], rely=measurement_lut[unit][3][1])
                    message_label.after(1250, message_label.destroy)
                    light_status = 1
                else:
                    light_status = 0

            elif unit == 'P':
                check_pressure()
                message_text = ""
                if open_timestamp != "":
                    message_text = "Prozor je zadnje otvaran/zatvaran u: " + open_timestamp
                message_label = tk.Label(self, text=message_text, font=MEDIUM_FONT)
                message_label.place(relx=measurement_lut[unit][3][0], rely=measurement_lut[unit][3][1])

    def update_time(self):
        time_label = tk.Label(self, text=datetime.now().strftime("%H:%M"), font=MEDIUM_FONT)
        time_label.place(relx=0.875, rely=0.04375)
        time_label.after(1500, time_label.destroy)

    def update_light(self):
        global LIGHT_THRESHOLD
        value = self.light_slider.get()
        LIGHT_THRESHOLD = value
        config.set('default', 'light_threshold', str(value))
        update_config()

    def update_temp(self):
        global TEMP_COMFORT_LOW, TEMP_COMFORT_HIGH
        low = self.temp_slider_lo.get()
        high = self.temp_slider_hi.get()
        self.temp_status.destroy()

        if low >= high:
            self.temp_slider_lo.set(TEMP_COMFORT_LOW)
            self.temp_slider_hi.set(TEMP_COMFORT_HIGH)
            self.temp_status = tk.Label(self, text="Donja granica ne može biti veća od gornje", font=SMALL_FONT)
            self.temp_status.place(x=500, y=666)
            return

        TEMP_COMFORT_LOW, TEMP_COMFORT_HIGH = low, high
        config.set('default', 'temperature_comfort_low', str(low))
        config.set('default', 'temperature_comfort_high', str(high))
        update_config()

    def update_hum(self):
        global HUM_COMFORT_LOW, HUM_COMFORT_HIGH
        low = self.hum_slider_lo.get()
        high = self.hum_slider_hi.get()
        self.hum_status.destroy()

        if low >= high:
            self.hum_slider_lo.set(HUM_COMFORT_LOW)
            self.hum_slider_hi.set(HUM_COMFORT_HIGH)
            self.hum_status = tk.Label(self, text="Donja granica ne može biti veća od gornje", font=SMALL_FONT)
            self.hum_status.place(x=900, y=666)
            return

        HUM_COMFORT_LOW, HUM_COMFORT_HIGH = low, high
        config.set('default', 'humidity_comfort_low', str(low))
        config.set('default', 'humidity_comfort_high', str(high))
        update_config()

    def update_serial(self):
        global serial_port, baud_rate
        port_string = self.port_box.get()
        port = port_string.split(" ")[0]
        serial_port = port
        baud = self.baud_box.get()
        baud_rate = int(baud)
        config.set('default', 'port', serial_port)
        config.set('default', 'baud', baud)
        update_config()

        serial_label = tk.Label(self, text="Ponovno pokrenite da bi se učitale promjene", font=SMALL_FONT)
        serial_label.place(relx=0.7, rely=0.35)


class GraphView(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text=f"Očitanja senzora u proteklih {BUFFER_LENGTH} minuta", font=LARGE_FONT)
        label.pack(padx=20, pady=30)

        button = tk.Button(self, text="Natrag", command=lambda: controller.show_frame(MainView))
        button.place(relx=0.07, rely=0.9)

        canvas = FigureCanvasTkAgg(figureTMP116, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.07, rely=0.125)

        canvas = FigureCanvasTkAgg(figureHDC2010TMP, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.35, rely=0.125)

        canvas = FigureCanvasTkAgg(figureDPS310TMP, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.64, rely=0.125)

        canvas = FigureCanvasTkAgg(figureHumidity, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.07, rely=0.5)

        canvas = FigureCanvasTkAgg(figureOPT, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.35, rely=0.5)

        canvas = FigureCanvasTkAgg(figurePressure, self)
        canvas.draw()
        canvas.get_tk_widget().place(relx=0.64, rely=0.5)

    def update_time(self):
        time_label = tk.Label(self, text=datetime.now().strftime("%H:%M"), font=MEDIUM_FONT)
        time_label.place(relx=0.875, rely=0.04375)
        time_label.after(1500, time_label.destroy)


class Application(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        tk.Tk.wm_title(self, APP_NAME)

        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in [MainView, GraphView]:
            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(MainView)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()

    def refresh_labels(self):
        self.frames[MainView].update_data()
        self.frames[MainView].check_actions()
        self.frames[MainView].update_time()
        self.frames[GraphView].update_time()


def call_repeatedly(interval, func, *args):
    stopped = threading.Event()

    def loop():
        while not stopped.wait(interval):
            func(*args)

    threading.Thread(target=loop).start()
    return stopped.set


def thread_gui():
    app = Application()
    app.geometry(f"{WINDOW_X}x{WINDOW_Y}")
    aniOPT = animation.FuncAnimation(figureOPT, animateOPT, interval=LIGHT_INTERVAL)
    aniPressure = animation.FuncAnimation(figurePressure, animatePressure, interval=PRESSURE_INTERVAL)
    aniHumidity = animation.FuncAnimation(figureHumidity, animateHumidity, interval=HUMIDITY_INTERVAL)
    aniTMP116 = animation.FuncAnimation(figureTMP116, animateTMP116, interval=TEMPERATURE_INTERVAL)
    aniHDC2010_TMP = animation.FuncAnimation(figureHDC2010TMP, animateHDC2010TMP, interval=TEMPERATURE_INTERVAL)
    aniDPS310_TMP = animation.FuncAnimation(figureDPS310TMP, animateDPS310TMP, interval=TEMPERATURE_INTERVAL)

    def thread_update():
        print("bingus")
        stop_refresh_labels = call_repeatedly(1, app.refresh_labels)
        if serial_status:
            stop_write_serial = call_repeatedly(0.25, write_serial)

    update_thread = threading.Thread(target=thread_update, name="update", daemon=True)
    update_thread.start()
    app.mainloop()
    sys.exit()


if __name__ == '__main__':
    file_check()
    check_serial()

    if serial_status:
        try:
            serial_connection = serial.Serial(serial_port, baud_rate, timeout=1)
            serial_connection.reset_input_buffer()
        except:
            print("Nema serijskog uređaja")
            sys.exit()

    gui_thread = threading.Thread(target=thread_gui, name="gui", daemon=True)
    gui_thread.start()

    while gui_thread.is_alive():
        time.sleep(0.5)
        if serial_status:
            if serial_connection.isOpen():
                read_serial()
                clean_buffer(csv_files)

    sys.exit()
