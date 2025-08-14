# ESP32 Bitcoin Price Fetcher

A professional ESP32 project built with ESP-IDF that fetches Bitcoin price data from CoinGecko API and displays it on an SSD1306 OLED display.

## Features

- **Real-time Bitcoin Price**: Fetches current Bitcoin price from CoinGecko API
- **24h Price Change**: Displays 24-hour price change percentage
- **OLED Display**: Beautiful visual interface with Bitcoin icon
- **WiFi Connectivity**: Automatic WiFi connection and reconnection
- **Button Control**: Toggle between active and standby modes
- **LED Status**: Visual feedback with onboard LED
- **Professional Architecture**: Built with ESP-IDF using FreeRTOS tasks

## Hardware Requirements

- ESP32 development board
- SSD1306 OLED display (128x64 pixels)
- Push button
- LED (optional, for status indication)
- Breadboard and jumper wires

## Pin Configuration

| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| SSD1306 SDA | GPIO 21 | I2C Data line |
| SSD1306 SCL | GPIO 22 | I2C Clock line |
| Button | GPIO 4 | Input with internal pull-up |
| LED | GPIO 7 | Output for status indication |

## Project Structure

```
bitcoin_price_fetcher/
├── CMakeLists.txt          # Main project CMake file
├── sdkconfig.defaults      # Default SDK configuration
├── main/
│   ├── CMakeLists.txt      # Main component CMake file
│   └── main.c             # Main application code
└── README.md              # This file
```

## Building and Flashing

### Prerequisites

1. Install ESP-IDF (v4.4 or later)
2. Set up ESP-IDF environment variables

### Build Commands

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

## Configuration

### WiFi Settings

Edit the WiFi credentials in `main/main.c`:

```c
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASS "Your_WiFi_Password"
```

### API Settings

The project uses CoinGecko's free API. You can modify the API endpoint in `main/main.c`:

```c
#define API_URL "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd&include_24hr_change=true"
```

## Usage

1. **Power On**: The ESP32 will connect to WiFi and show a welcome screen
2. **Press Button**: Toggle between active mode (fetching data) and standby
3. **Active Mode**: 
   - LED turns on
   - Fetches Bitcoin price every 10 minutes
   - Displays current price and 24h change
4. **Standby Mode**: 
   - LED turns off
   - Shows standby screen
   - No API calls

## Technical Details

### Architecture

- **FreeRTOS Tasks**: Separate tasks for button handling and main logic
- **Event-Driven WiFi**: Uses ESP-IDF event system for WiFi management
- **HTTP Client**: Built-in ESP-IDF HTTP client for API requests
- **I2C Communication**: Direct I2C communication with SSD1306 display

### Key Components

- **WiFi Manager**: Handles connection, reconnection, and status monitoring
- **HTTP Client**: Manages API requests with proper error handling
- **Display Driver**: Low-level SSD1306 communication
- **Button Handler**: Debounced button input with state management

### Memory Management

- Uses static allocation where possible
- Proper task stack sizing
- Efficient string handling

## Troubleshooting

### Common Issues

1. **WiFi Connection Failed**
   - Check SSID and password
   - Verify WiFi network availability
   - Check serial monitor for error messages

2. **Display Not Working**
   - Verify I2C connections (SDA/SCL)
   - Check I2C address (default: 0x3C)
   - Ensure proper power supply

3. **Build Errors**
   - Verify ESP-IDF installation
   - Check component dependencies
   - Ensure proper CMake configuration

### Debug Information

The project includes comprehensive logging:
- WiFi connection status
- HTTP request results
- Display operations
- Button events

Use `idf.py monitor` to view real-time logs.

## Future Enhancements

- [ ] Add more cryptocurrencies
- [ ] Implement price alerts
- [ ] Add historical price charts
- [ ] Web configuration interface
- [ ] MQTT integration for IoT
- [ ] Battery power management

## Contributing

This is a learning project for ESP-IDF development. Feel free to:
- Report issues
- Suggest improvements
- Submit pull requests
- Share your modifications

## License

This project is open source and available under the MIT License.

## Acknowledgments

- Espressif Systems for ESP-IDF
- CoinGecko for the free cryptocurrency API
- Adafruit for the original SSD1306 library concept

---

**Note**: This project demonstrates professional ESP-IDF development practices and serves as a foundation for more complex embedded applications. 