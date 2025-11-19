# esp-idf-c6-components

A collection of sensor and actuator wrapper libraries for the ESP32-C6, built with ESP-IDF.  
This repository provides easy-to-use C++ classes for various hardware components like DHT11, buttons, joysticks, LED strips, KY038 sensors, and WiFi utilities.

---

## 🚀 Features
- DHT11 sensor wrapper (temperature & humidity)
- Button input wrapper
- Joystick input wrapper
- KY038 sound sensor wrapper
- LED strip control wrapper
- WiFi helper utilities
- Easy-to-use C++ classes with ESP-IDF integration

---

## 📦 Installation

Clone the repository into your ESP-IDF project `lib` folder:

```bash
cd your-esp-idf-project/lib
git clone https://github.com/Felix-Brodmann/esp-idf-c6-components.git
```

Then, include the necessary headers in your source files.

This project was developed and tested with PlatformIO using the ESP-IDF framework for the ESP32-C6.

---

## 🛠 Usage

Include the headers in your project and use the classes:


```cpp
#include "dht-11.hpp"
#include "button.hpp"

Dht11 sensor(GPIO_NUM_4);
if (esp_err_t result = sensor.read(); result != ESP_OK) {
    // Handle error
}
float temperature = sensor.get_temperature();
float humidity = sensor.get_humidity();
```

For other components, check their respective headers in `lib/<component_name>/include`.

---

## 📁 Project Structure

```
your-project/
├── components/
│   ├── dht11/
│   ├── button/
│   ├── joystick/
│   ├── ky038/
│   ├── ledstrip/
│   └── wifi/
├── README.md
└── LICENSE
```

---

## ⚙ Configuration

The esp-idf-lib/dht and espressif/led_strip components are used, therefore make sure you add following dependencies to your `idf_component.yml`:

```yml
dependencies:
  esp-idf-lib/dht: "^1.1.7"
  espressif/led_strip: "^3"
```

The Display component simply wraps the initialization of the LovyanGFX library, so
you need to add following line to you `platformio.ini`:

```ini
lib_deps = https://github.com/lovyan03/LovyanGFX.git#develop
```

---

## 🤝 Contributing

Contributions are welcome!

1. Fork the repo  
2. Create a new branch  
3. Commit your changes  
4. Open a Pull Request  

---

## 📜 License

This project is licensed under the MIT License.  
See the `LICENSE` file for details.
