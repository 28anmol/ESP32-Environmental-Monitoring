# 🌎 ESP32 Environmental Monitoring System 🚀

This project is WiFi-enabled real-time environmental monitoring system built on ESP32 WROOM 32D microcontroller. It captures and displays key atmospheric parameters such as **temperature**🌡️, **humidity**💧, **PM2.5 dust density**, and **air quality level** using various sensors. This data is displayed locally on a **128x64 OLED display** and also served through a responsive **web interface** over the local network.


## 🚀 Features
- 📡 **WiFi Based Server**: Hosts a dynamic webpage accessible to any device on the same network
- 🌡️ **Temperature monitoring**: Captures accurate ambient temperature using digital temperature sensor
- 💧 **Humidity Monitoring**: uses humidity sensor for relative humidity data
- 🌫️ **PM2.5 Air Quality Measurement**: Uses dust density sensor to estimate PM2.5 levels
- 🕙 **Live Clock with NTP**: Fetches real time date and time using NTP(Network Time Protocol)
- 📺 **OLED Display Output**: Display the readings on a OLED screen
- 🔁 **Timed sensor Updates**: Reads and updates sensor values after every 60 seconds
- 🌐 **Responsive Web Dashboard**: Presents clean and styled HTML output of live data


## 🔧 Hardware Used
| 🧩 Component | 🔍 Description | 📦 Model/Type |
|--------------|----------------|--------------|
| 🧠 Microcontroller | Main controller of the system - with WiFi & Bluetooth | ESP32 WROOM 32D |
| 🌡️ Temperature Sensor | To measure ambient temperature| Dallas Instruments DS18B20 Digital Temperature Sensor|
| 💧 Humidity Sensor | To measure ambient humidity | DHT11 Sensor |
| 🌫️ Dust Sensor | To measure suspended 2.5um particulate matter in air| GP2Y1010AU0F PM2.5 Sensor |
| 📺 Display Screen | To display sensor readings locally | SSD1306 driver based 0.96in 128x64 monochromatic(Blue) SPI/I2C OLED display |
| 🔌  Electrical Components | Prototyping components | Dupont Jumper wires(M-M, M-F, F-F), Resistors, Capacitor, Breadboard, acrylic board|
| 🔋 Power Supply| To power the whole system | 5V USB adapter |
| 🛠️ Multimeter | For voltage, current & continuity testing | Kaiweets HT118E 20000 count Digital Multimeter |
| 🪛 Soldering Iron | For assembling and soldering components | 25W soldering iron |

## 🕓 Time Sync via NTP
- **NTP Server**: pool.ntp.org
- **Timezone**: GMT + 5:30(Indian Standard Time)
- Displays synchronized **date**, **time**, and **day of the week**

## 🧪 Air Quality Mapping Logic
| PM2.5 Range (µg/m³) | Air Quality Level |
|---------------------|-------------------|
| 0 - 12 | Clean Air |
| 12.1 - 35.4 | Moderate |
| 35.5 - 55.4 | Unhealthy for Sensitive Groups |
| 55.5 - 150.4 | Unhealthy |
| 150.5 - 250.4 | Very Unhealthy |
| 250.5+ | Hazardous |

## 📸 Screenshot




