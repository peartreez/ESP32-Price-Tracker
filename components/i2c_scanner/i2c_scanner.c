#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "I2C_SCANNER";

// I2C Configuration - same as main project
#define I2C_MASTER_SCL_IO 9   // Updated to match schematic (GPIO 9)
#define I2C_MASTER_SDA_IO 8   // Updated to match schematic (GPIO 8)
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

// I2C master initialization
static void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

// Scan for I2C devices
static void i2c_scanner(void)
{
    ESP_LOGI(TAG, "Starting I2C Scanner...");
    ESP_LOGI(TAG, "Scanning I2C bus on pins SDA:%d, SCL:%d", I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            fflush(stdout);
            
            int addr = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_stop(cmd);
            
            esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100));
            i2c_cmd_link_delete(cmd);
            
            if (ret == ESP_OK) {
                printf("%02x ", addr);
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }
    
    ESP_LOGI(TAG, "I2C Scanner completed!");
}

// Main task
static void scanner_task(void *pvParameter)
{
    while(1) {
        i2c_scanner();
        ESP_LOGI(TAG, "Waiting 10 seconds before next scan...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "I2C Scanner for ESP32 Bitcoin Price Tracker");
    ESP_LOGI(TAG, "This will help you test your OLED display connection");
    
    // Initialize I2C
    i2c_master_init();
    ESP_LOGI(TAG, "I2C initialized successfully");
    
    // Create scanner task
    xTaskCreate(scanner_task, "scanner_task", 2048, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "I2C Scanner task created. Check serial output for results.");
    ESP_LOGI(TAG, "Expected to see device at address 0x3C (SSD1306 OLED)");
} 