# Changelog

All notable changes to the ESP32 Bitcoin Price Tracker project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Breadboard testing mode flag in main.c
- I2C scanner component for OLED display testing
- Comprehensive breadboard testing guide
- Test mode delays for component verification
- Enhanced project structure with components directory

### Changed
- Updated main CMakeLists.txt to include components directory
- Enhanced main.c with breadboard testing features
- Improved project documentation and testing procedures

## [0.2.0] - 2024-12-19

### Added
- Complete ESP-IDF project structure
- Professional CMake configuration
- FreeRTOS task-based architecture
- Event-driven WiFi management
- HTTP client for CoinGecko API integration
- I2C communication with SSD1306 OLED display
- GPIO configuration for button and LED
- Comprehensive error handling and logging
- Professional project documentation
- Git repository setup and version control

### Changed
- Converted Arduino code to ESP-IDF C
- Replaced Arduino libraries with native ESP-IDF APIs
- Restructured code for professional embedded development
- Implemented proper memory management and task scheduling

### Technical Improvements
- **Architecture**: FreeRTOS tasks for button handling and main logic
- **WiFi**: Event-driven connection management with automatic reconnection
- **HTTP**: Built-in ESP-IDF HTTP client with proper error handling
- **Display**: Direct I2C communication with SSD1306
- **GPIO**: Professional GPIO configuration with proper pull-up/down settings
- **Logging**: Comprehensive ESP-IDF logging system

## [0.1.0] - 2024-12-19

### Added
- Initial Arduino-based Bitcoin price fetcher
- WiFi connectivity for API calls
- SSD1306 OLED display support
- Button control for start/stop functionality
- LED status indication
- CoinGecko API integration
- Basic error handling and user feedback

### Features
- Real-time Bitcoin price fetching
- 24-hour price change display
- Automatic WiFi connection
- Button-controlled operation modes
- Visual feedback with LED and display

## Development Notes

### Version Naming Convention
- **0.x.x**: Development versions
- **1.0.0**: First stable release
- **x.y.z**: Major.Minor.Patch

### Next Planned Features
- [ ] Complete OLED display text rendering
- [ ] JSON response parsing for API data
- [ ] Price formatting and display optimization
- [ ] Error recovery mechanisms
- [ ] Power management features
- [ ] Web configuration interface
- [ ] MQTT integration for IoT

### Known Issues
- OLED display text rendering needs completion
- JSON parsing implementation pending
- Error handling could be enhanced
- Power consumption optimization needed

### Testing Status
- [x] Project structure and build system
- [x] GPIO configuration
- [x] I2C initialization
- [x] WiFi connection
- [x] HTTP client setup
- [ ] OLED display functionality
- [ ] Button input handling
- [ ] API data processing
- [ ] End-to-end functionality

---

## Contributing

When contributing to this project, please update this changelog with your changes following the established format.

## License

This project is licensed under the MIT License - see the LICENSE file for details. 