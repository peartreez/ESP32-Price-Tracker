# ESP32 Bitcoin Price Fetcher

A professional ESP32 project built with ESP-IDF that fetches Bitcoin price data from CoinGecko API and displays it on an SSD1306 OLED display.

## 🚧 **Development Status: Beta v0.2.0**

**Current Version**: 0.2.0 (Development)  
**Last Updated**: December 19, 2024  
**Status**: Ready for breadboard testing and development

> **⚠️ Note**: This is a development version. Core functionality is implemented but some features (OLED text rendering, JSON parsing) are pending completion.

## ✨ **Features**

- **Real-time Bitcoin Price**: Fetches current Bitcoin price from CoinGecko API
- **24h Price Change**: Displays 24-hour price change percentage
- **OLED Display**: Beautiful visual interface with Bitcoin icon
- **WiFi Connectivity**: Automatic WiFi connection and reconnection
- **Button Control**: Toggle between active and standby modes
- **LED Status**: Visual feedback with onboard LED
- **Professional Architecture**: Built with ESP-IDF using FreeRTOS tasks
- **Breadboard Testing**: Built-in test mode for component verification

## 🧪 **Breadboard Testing Ready**

This project includes comprehensive breadboard testing support:

- **Test Mode Flag**: Enable breadboard testing with `#define BREADBOARD_TEST_MODE 1`
- **I2C Scanner Component**: Test OLED display connections
- **Testing Guide**: Complete breadboard setup instructions
- **Component Verification**: Built-in delays and logging for testing

📖 **See [BREADBOARD_TESTING.md](BREADBOARD_TESTING.md) for detailed setup instructions**

## 🔧 **Hardware Requirements**

- ESP32-C3-MINI-1 development board
- SSD1306 OLED display (128x64 pixels, I2C)
- Push button (momentary, normally open)
- LED (any color, for status indication)
- 220Ω resistor (for LED current limiting)
- Breadboard and jumper wires
- Micro USB cable

## 📍 **Pin Configuration (Based on Schematic)**

| Component | ESP32-C3-MINI-1 Pin | Description |
|-----------|---------------------|-------------|
| SSD1306 SDA | GPIO 8 | I2C Data line |
| SSD1306 SCL | GPIO 9 | I2C Clock line |
| Button | GPIO 4 | Input with internal pull-up |
| LED | GPIO 7 | Output for status indication |

**Note**: The schematic shows ESP32-C3-MINI-1, which has different pin assignments than standard ESP32 boards.

## 🔌 **Schematic & Wiring**

### **Breadboard Layout**

The project includes detailed breadboard wiring diagrams and schematics to help you set up your components correctly.

![Breadboard Schematic](assets/breadboard-schematic.png)

### **Connection Details (Based on Schematic)**

- **Power**: 5V and GND from ESP32-C3-MINI-1 to breadboard power rails
- **I2C**: SDA (GPIO 8) and SCL (GPIO 9) for OLED display
- **Button**: GPIO 4 with connection to GND (internal pull-up will be used)
- **LED**: GPIO 7 through 220Ω resistor to GND

### **Wire Color Convention (From Schematic)**

- **Black wires**: Ground (GND) connections
- **Red wires**: Power (5V) connections  
- **Green wires**: Data/Signal connections (I2C SCL/SDA, GPIO for button and LED)

### **Component Layout**

![Component Layout](assets/component-layout.png)

### **Wiring Diagram**

![Wiring Diagram](assets/wiring-diagram.png)

### **Schematic Notes**

Based on your schematic diagram:
- **ESP32-C3-MINI-1** is used (not standard ESP32)
- **OLED VCC** connects to **5V** (not 3.3V as in standard ESP32)
- **Button** uses internal pull-up (no external resistor shown)
- **LED** has 220Ω resistor for current limiting
- **I2C pins** are GPIO 8 (SDA) and GPIO 9 (SCL)

## 📁 **Project Structure**

```
bitcoin_price_fetcher/
├── CMakeLists.txt              # Main project CMake file
├── sdkconfig.defaults          # Default SDK configuration
├── main/
│   ├── CMakeLists.txt          # Main component CMake file
│   └── main.c                  # Main application code
├── components/
│   └── i2c_scanner/            # I2C scanner for testing
│       ├── CMakeLists.txt      # Component CMake file
│       └── i2c_scanner.c       # I2C scanner implementation
├── BREADBOARD_TESTING.md       # Breadboard testing guide
├── CHANGELOG.md                # Development changelog
├── README.md                   # This file
└── .gitignore                  # Git exclusions
```

## 🚀 **Quick Start**

### Prerequisites

1. **ESP-IDF v4.4 or later** installed and configured
2. **Hardware components** ready for breadboard testing
3. **WiFi credentials** configured in `main/main.c`

### Build and Test

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Build the project
idf.py build

# Flash to ESP32
idf.py flash

# Monitor serial output
idf.py monitor
```

### Combined Commands

```bash
# Build, flash, and monitor in one command
idf.py build flash monitor
```

## ⚙️ **Configuration**

### WiFi Settings

Edit the WiFi credentials in `main/main.c`:

```c
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASS "Your_WiFi_Password"
```

### Test Mode

Enable breadboard testing mode in `main/main.c`:

```c
#define BREADBOARD_TEST_MODE 1  // Set to 1 for testing
```

### API Settings

The project uses CoinGecko's free API. You can modify the API endpoint in `main/main.c`:

```c
#define API_URL "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd&include_24hr_change=true"
```

## 🧪 **Testing Workflow**

### 1. **Breadboard Setup**
- Follow [BREADBOARD_TESTING.md](BREADBOARD_TESTING.md)
- Connect all components according to pin configuration
- Verify power and ground connections

### 2. **I2C Scanner Test**
- Build and flash the I2C scanner component
- Verify OLED display is detected at address 0x3C
- Check serial output for device detection

### 3. **Main Project Test**
- Enable test mode in main.c
- Flash the main project
- Monitor serial output for initialization messages
- Test button functionality and LED response

### 4. **WiFi and API Test**
- Verify WiFi connection
- Test API calls and response handling
- Check display updates

## 📊 **Current Development Status**

| Component | Status | Notes |
|-----------|--------|-------|
| Project Structure | ✅ Complete | Professional ESP-IDF setup |
| GPIO Configuration | ✅ Complete | Button and LED working |
| I2C Communication | ✅ Complete | SSD1306 initialization |
| WiFi Management | ✅ Complete | Event-driven connection |
| HTTP Client | ✅ Complete | API integration ready |
| OLED Display | 🟡 Partial | Basic commands, text rendering pending |
| JSON Parsing | 🟡 Partial | Structure ready, implementation pending |
| Button Handling | 🟡 Partial | GPIO working, debouncing implemented |
| Error Handling | 🟡 Partial | Basic logging, recovery pending |

## 🛠️ **Troubleshooting**

### Common Issues

1. **WiFi Connection Failed**
   - Check SSID and password
   - Verify WiFi network availability
   - Check serial monitor for error messages

2. **Display Not Working**
   - Use I2C scanner to verify connection
   - Check I2C connections (SDA/SCL)
   - Verify power supply (3.3V, not 5V)
   - Check I2C address (default: 0x3C)

3. **Build Errors**
   - Verify ESP-IDF installation
   - Check component dependencies
   - Ensure proper CMake configuration

4. **Button Not Responding**
   - Check GPIO pin connection
   - Verify button orientation
   - Test with multimeter for continuity
   - Check internal pull-up configuration

### Debug Information

The project includes comprehensive logging:
- WiFi connection status
- HTTP request results
- Display operations
- Button events
- Test mode information

Use `idf.py monitor` to view real-time logs.

## 🔮 **Roadmap**

### **v0.3.0** (Next Release)
- [ ] Complete OLED text rendering
- [ ] Implement JSON response parsing
- [ ] Add price formatting and display
- [ ] Enhance error handling

### **v1.0.0** (First Stable Release)
- [ ] Full functionality testing
- [ ] Performance optimization
- [ ] Documentation completion
- [ ] Example configurations

### **Future Enhancements**
- [ ] Add more cryptocurrencies
- [ ] Implement price alerts
- [ ] Add historical price charts
- [ ] Web configuration interface
- [ ] MQTT integration for IoT
- [ ] Battery power management

## 📚 **Documentation**

- **[BREADBOARD_TESTING.md](BREADBOARD_TESTING.md)** - Complete breadboard setup guide
- **[CHANGELOG.md](CHANGELOG.md)** - Development history and version tracking
- **[ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)** - Framework reference

## 🤝 **Contributing**

This is a learning project for ESP-IDF development. We welcome contributions:

- Report issues and bugs
- Suggest improvements and features
- Submit pull requests
- Share your modifications and experiences

## 📄 **License**

This project is open source and available under the MIT License.

## 🙏 **Acknowledgments**

- **The DIY Life** - Inspired this project and provided the original concept
- **Espressif Systems** for ESP-IDF
- **CoinGecko** for the free cryptocurrency API
- **Adafruit** for the original SSD1306 library concept
- **FreeRTOS** for the real-time operating system

---

## 📝 **Development Notes**

**Current Focus**: Completing OLED display functionality and JSON parsing  
**Testing Priority**: Breadboard verification of all components  
**Next Milestone**: v0.3.0 with full display functionality  

**Note**: This project demonstrates professional ESP-IDF development practices and serves as a foundation for more complex embedded applications. The development approach prioritizes learning and best practices over quick functionality.
