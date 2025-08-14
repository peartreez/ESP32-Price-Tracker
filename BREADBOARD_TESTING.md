# Breadboard Testing Guide

## 🧪 **Breadboard Setup for ESP32 Bitcoin Price Tracker**

This guide will help you test your project on a breadboard before soldering the final build.

## 📋 **Required Components for Testing**

### **Core Components**
- [ ] ESP32 development board (ESP32 DevKit, NodeMCU-32S, etc.)
- [ ] SSD1306 OLED display (128x64 pixels, I2C)
- [ ] Push button (momentary, normally open)
- [ ] LED (any color, for status indication)
- [ ] 220Ω resistor (for LED current limiting)
- [ ] 10kΩ resistor (for button pull-up, though ESP32 has internal pull-up)

### **Breadboard & Wiring**
- [ ] Breadboard (830 tie-points recommended)
- [ ] Jumper wires (male-to-male, male-to-female)
- [ ] Micro USB cable (for ESP32 programming)
- [ ] Power supply (USB from computer or 5V power supply)

## 🔌 **Breadboard Wiring Diagram**

```
ESP32-C3-MINI-1 Board    Breadboard          Component
----------------------    -----------          ---------
5V               ----->  Power Rail (+)      Power for components
GND              ----->  Power Rail (-)      Common ground
GPIO 8           ----->  SSD1306 SDA         I2C Data line
GPIO 9           ----->  SSD1306 SCL         I2C Clock line
GPIO 4           ----->  Button Pin 1        Button input
Button Pin 2     ----->  Power Rail (+)     5V (with internal pull-up)
GPIO 7           ----->  LED Anode (+)       LED control
LED Cathode (-)  --->  220Ω Resistor --->  Power Rail (-)
```

## 📍 **Detailed Pin Connections**

### **Power Connections**
- **ESP32-C3-MINI-1 5V** → Breadboard power rail (+)
- **ESP32-C3-MINI-1 GND** → Breadboard power rail (-)

### **SSD1306 OLED Display (I2C)**
- **ESP32-C3-MINI-1 GPIO 8** → **SSD1306 SDA** (Data line)
- **ESP32-C3-MINI-1 GPIO 9** → **SSD1306 SCL** (Clock line)
- **SSD1306 VCC** → Breadboard power rail (+) (5V)
- **SSD1306 GND** → Breadboard power rail (-)

### **Push Button**
- **ESP32-C3-MINI-1 GPIO 4** → **Button Pin 1**
- **Button Pin 2** → Breadboard power rail (+) (5V)
- **Note**: ESP32-C3-MINI-1 has internal pull-up, but you can add external 10kΩ for reliability

### **Status LED**
- **ESP32-C3-MINI-1 GPIO 7** → **220Ω Resistor** → **LED Anode (+)**
- **LED Cathode (-)** → Breadboard power rail (-)

## ⚠️ **Important Safety Notes**

1. **Voltage Levels**: ESP32 operates at 3.3V - never connect 5V components directly
2. **Current Limiting**: Always use a resistor with LEDs (220Ω recommended)
3. **Power Supply**: Ensure stable 3.3V supply - USB from computer is usually fine
4. **ESD Protection**: Handle components carefully to avoid static damage

## 🔍 **Testing Checklist**

### **Before Power-On**
- [ ] Double-check all connections
- [ ] Verify power and ground connections
- [ ] Ensure no loose wires or shorts
- [ ] Check component orientation (LED, button)

### **Power-On Tests**
- [ ] ESP32 powers up (power LED lights)
- [ ] No smoke or unusual smells
- [ ] All components receive power
- [ ] Serial monitor shows boot messages

### **Functionality Tests**
- [ ] WiFi connects successfully
- [ ] Button responds to presses
- [ ] LED changes state with button
- [ ] OLED display shows content
- [ ] API calls work (check serial output)

## 🛠️ **Troubleshooting Common Issues**

### **Display Not Working**
- Check I2C connections (SDA/SCL)
- Verify power supply (3.3V, not 5V)
- Check I2C address (default: 0x3C)
- Use I2C scanner to detect device

### **Button Not Responding**
- Check GPIO pin connection
- Verify button orientation
- Test with multimeter for continuity
- Check internal pull-up configuration

### **WiFi Issues**
- Verify SSID and password in code
- Check WiFi network availability
- Monitor serial output for error messages
- Test with simple WiFi example first

### **Build/Flash Issues**
- Ensure ESP-IDF is properly installed
- Check USB cable (data + power)
- Verify board selection in ESP-IDF
- Check serial port permissions

## 📱 **I2C Scanner for Testing**

Before running your main project, test I2C communication:

```bash
# Build and flash the I2C scanner example
cd $IDF_PATH/examples/peripherals/i2c/i2c_simple
idf.py build flash monitor
```

This will help verify your OLED display is properly connected.

## 🚀 **Next Steps After Testing**

1. **Verify all functionality works** on breadboard
2. **Document any issues** and solutions
3. **Optimize component placement** for final build
4. **Plan PCB layout** if going that route
5. **Prepare for soldering** with confidence

## 📚 **Additional Resources**

- [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- [I2C Communication Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)
- [GPIO Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)

---

**Happy Testing!** 🧪✨

Remember: Breadboard testing saves time and components. Take your time and verify everything works before moving to the final build. 