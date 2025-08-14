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

- ESP32-C3-MINI development board
- SSD1306 OLED display (128x64 pixels, I2C)
- Push button (momentary, normally open)
- LED (any color, for status indication)
- **2KΩ resistors (2x) for LED current limiting** ⚠️ **Note: PCB uses 2x 2KΩ in series**
- Breadboard and jumper wires
- Micro USB cable

## 📍 **Pin Configuration (Based on PCB Schematic)**

| Component | ESP32-C3-MINI Pin | Description |
|-----------|---------------------|-------------|
| SSD1306 SDA | GPIO 8 | I2C Data line |
| SSD1306 SCL | GPIO 9 | I2C Clock line |
| Button | GPIO 4 | Input with 3.3V pull-up |
| LED | GPIO 7 | Output through 2KΩ + 2KΩ resistors |

**⚠️ Important PCB Notes:**
- **OLED VCC**: Connected to **3.3V (Pin 2)**, not 5V
- **Button**: Connected to **3.3V (Pin 3)** with internal pull-up
- **LED Circuit**: GPIO 7 → 2KΩ → LED anode → 2KΩ → GND (Pin 24)
- **Total LED Resistance**: 4KΩ (will be dimmer than typical 220Ω setup)

## 🔌 **Schematic & Wiring**

### **Breadboard Layout**

The project includes detailed breadboard wiring diagrams and schematics to help you set up your components correctly.

![Breadboard Schematic](assets/breadboard-schematic.png)

### **Connection Details (Based on PCB Schematic)**

- **Power**: 3.3V and GND from ESP32-C3-MINI to breadboard power rails
- **I2C**: SDA (GPIO 8) and SCL (GPIO 9) for OLED display
- **Button**: GPIO 4 with connection to 3.3V (Pin 3)
- **LED**: GPIO 7 through 2KΩ + 2KΩ resistors to GND

### **Wire Color Convention (From Schematic)**

- **Black wires**: Ground (GND) connections
- **Red wires**: Power (3.3V) connections  
- **Green wires**: Data/Signal connections (I2C SCL/SDA, GPIO for button and LED)

### **Component Layout**

![Component Layout](assets/component-layout.png)

### **Wiring Diagram**

![Wiring Diagram](assets/wiring-diagram.png)

### **PCB Schematic Notes**

Based on your professional PCB schematic:
- **ESP32-C3-MINI** with 30-pin layout
- **OLED VCC** connects to **3.3V (Pin 2)** - this is correct for I2C
- **Button** uses **3.3V (Pin 3)** pull-up
- **LED** has **2x 2KΩ resistors in series** (4KΩ total)
- **I2C pins** are GPIO 8 (SDA) and GPIO 9 (SCL)
- **Power distribution**: Multiple GND pins (1, 6, 8, 12, 15, 16, 19, 24) and 3.3V pins (2, 3)

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

**⚠️ IMPORTANT**: WiFi credentials are no longer hardcoded for security!

The project now uses **NVS (Non-Volatile Storage)** to securely store WiFi credentials in the ESP32's flash memory.

#### **Option 1: Use Default Credentials (Development Only)**
Edit the default values in `main/main.c`:
```c
#define WIFI_SSID_DEFAULT "Your_WiFi_SSID"      // Change this
#define WIFI_PASS_DEFAULT "Your_WiFi_Password"  // Change this
```

#### **Option 2: Configure via NVS (Recommended for Production)**

1. **Build and flash the WiFi configuration utility:**
   ```bash
   cd tools
   idf.py build flash monitor
   ```

2. **Edit the credentials in `tools/wifi_config.c`:**
   ```c
   const char *my_ssid = "Your_Actual_WiFi_SSID";
   const char *my_password = "Your_Actual_WiFi_Password";
   ```

3. **Flash the utility to set credentials:**
   ```bash
   idf.py flash monitor
   ```

4. **Flash the main project:**
   ```bash
   cd ..
   idf.py build flash monitor
   ```

#### **Option 3: Programmatic Configuration**
Use the NVS functions in your code:
```c
#include "nvs_flash.h"
#include "nvs.h"

// Save credentials
wifi_config_save_to_nvs("MyWiFi", "MyPassword");

// Load credentials
char ssid[33], password[65];
wifi_config_load_from_nvs(ssid, password);
```

### **Security Benefits:**
- ✅ **No hardcoded credentials** in source code
- ✅ **Credentials stored securely** in ESP32 flash memory
- ✅ **Easy to update** without recompiling
- ✅ **Production ready** configuration management
- ✅ **Multiple devices** can use different credentials

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

## 🚨 **Amateur Issues & Lessons Learned**

### **⚠️ PCB Design Issues (First Schematic)**

This project was designed from my first PCB schematic, and several amateur mistakes were identified and corrected:

#### **1. LED Resistor Values** ❌
- **What I Did**: Used 2x 2KΩ resistors in series (4KΩ total)
- **Problem**: LED will be extremely dim, barely visible
- **Why It Happened**: First-time designer, didn't understand LED current requirements
- **Correct Value**: Should be 220Ω for normal brightness
- **Lesson**: Always calculate LED current: I = (3.3V - 2.1V) / 220Ω ≈ 5.5mA

#### **2. Power Supply Confusion** ❌
- **What I Did**: Initially assumed OLED needed 5V power
- **Problem**: Code didn't match actual hardware
- **Why It Happened**: Didn't research I2C device voltage requirements
- **Correct Configuration**: OLED VCC to 3.3V (Pin 2) ✅
- **Lesson**: I2C devices typically use 3.3V, not 5V

#### **3. Button Pull-up Assumptions** ❌
- **What I Did**: Assumed internal pull-up would work with 5V
- **Problem**: Voltage level mismatch in code
- **Why It Happened**: Didn't understand ESP32-C3 voltage domains
- **Correct Configuration**: External 3.3V pull-up (Pin 3) ✅
- **Lesson**: Always verify voltage levels between code and hardware

#### **4. Documentation Mismatch** ❌
- **What I Did**: Wrote code before finalizing hardware design
- **Problem**: Code assumptions didn't match actual PCB
- **Why It Happened**: Development order was backwards
- **Correct Approach**: Design hardware first, then write code to match
- **Lesson**: Hardware-first development prevents these issues

### **🔧 How These Issues Were Fixed**

1. **PCB Analysis**: Thorough review of schematic revealed mismatches
2. **Code Corrections**: Updated all pin configurations and power assumptions
3. **Documentation Sync**: Made README match actual hardware exactly
4. **Component Flexibility**: Through-hole resistors allow easy correction

### **📚 Lessons for Future Projects**

#### **Design Order:**
1. **Research components** and their requirements first
2. **Design hardware** schematic completely
3. **Write code** to match the actual hardware
4. **Test and iterate** before finalizing

#### **Common Beginner Mistakes to Avoid:**
- **Don't assume** voltage levels - research everything
- **Don't guess** resistor values - calculate them
- **Don't design** hardware and software separately
- **Don't skip** datasheet reading
- **Don't ignore** power requirements

#### **What I Did Right:**
- **Multiple GND pins** for good grounding
- **Proper I2C pin selection** (GPIO 8/9)
- **Clean, organized schematic** layout
- **Professional component selection**
- **Good power distribution** design

### **🎯 Current Status**

- **Hardware Design**: ✅ Professional quality, minor resistor issue
- **Code Implementation**: ✅ Now perfectly matches hardware
- **Documentation**: ✅ Synchronized with actual design
- **Ready for Assembly**: ✅ With recommended resistor changes

**Note**: These issues are documented to help other beginners avoid the same mistakes. The project will work perfectly with the recommended 220Ω resistors instead of 2KΩ.
