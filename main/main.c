#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "cJSON.h"

// Test Configuration - Set to 1 for breadboard testing
#define BREADBOARD_TEST_MODE 1

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define I2C_MASTER_SCL_IO 9   // Updated to match schematic (GPIO 9)
#define I2C_MASTER_SDA_IO 8   // Updated to match schematic (GPIO 8)
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

// Pin Configuration
#define BUTTON_PIN GPIO_NUM_4  // Button pin (matches schematic - connected to 3.3V)
#define LED_PIN GPIO_NUM_7     // LED pin (matches schematic - through 2KΩ + 2KΩ resistors)

// Hardware Configuration Notes (from PCB schematic):
// - OLED VCC: 3.3V (Pin 2), not 5V
// - Button: Connected to 3.3V (Pin 3) with internal pull-up
// - LED: GPIO 7 → 2KΩ resistor → LED anode → 2KΩ resistor → GND (Pin 24)
// - Total LED resistance: 4KΩ (will be dimmer than expected)

// WiFi Configuration - Now configurable via NVS
#define WIFI_SSID_DEFAULT "Your_WiFi_SSID"  // Default fallback
#define WIFI_PASS_DEFAULT "Your_WiFi_Password"  // Default fallback
#define WIFI_MAX_RETRY_COUNT 5
#define WIFI_RETRY_DELAY_MS 1000

// NVS Keys for WiFi credentials
#define NVS_NAMESPACE "wifi_config"
#define NVS_KEY_SSID "ssid"
#define NVS_KEY_PASS "password"

// API Configuration
#define API_URL "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd&include_24hr_change=true"

// Bitcoin Icon (24x24px)
static const unsigned char bitcoin_icon[] = {
    0x00, 0x7e, 0x00, 0x03, 0xff, 0xc0, 0x07, 0x81, 0xe0, 0x0e, 0x00, 0x70, 0x18, 0x28, 0x18, 0x30,
    0x28, 0x0c, 0x70, 0xfc, 0x0e, 0x60, 0xfe, 0x06, 0x60, 0xc7, 0x06, 0xc0, 0xc3, 0x03, 0xc0, 0xc7,
    0x03, 0xc0, 0xfe, 0x03, 0xc0, 0xff, 0x03, 0xc0, 0xc3, 0x83, 0xc0, 0xc1, 0x83, 0xc0, 0x60, 0xc3, 0x86,
    0x60, 0xff, 0x06, 0x70, 0xfe, 0x0e, 0x30, 0x28, 0x0c, 0x18, 0x28, 0x18, 0x0e, 0x00, 0x70, 0x07,
    0x81, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0x7e, 0x00
};

// Global Variables
static const char *TAG = "BITCOIN_FETCHER";
static bool display_active = false;
static bool fetch_in_progress = false;
static char last_price[32] = "";
static int64_t last_fetch_time = 0;
static const int64_t fetch_cooldown = 600000000; // 10 minutes in microseconds
static int64_t last_button_press = 0;
static const int64_t debounce_delay = 200000; // 200ms in microseconds

// Event group for WiFi
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

// HTTP client handle
static esp_http_client_handle_t http_client;

// Function declarations
static void wifi_init_sta(void);
static void i2c_master_init(void);
static void gpio_init(void);
static void display_init(void);
static void show_welcome_screen(void);
static void display_bitcoin_data(const char *price, double change_24h);
static void display_error(const char *error_msg);
static void display_message(const char *title, const char *message);
static void display_standby(void);
static void fetch_bitcoin_data(void);
static void print_center(const char *buf, int x, int y);
static void ssd1306_command(uint8_t cmd);
static void ssd1306_data(uint8_t data);
static void ssd1306_clear(void);
static void ssd1306_display(void);
static void ssd1306_draw_bitmap(int x, int y, const unsigned char *bitmap, int w, int h);

// WiFi configuration functions
static esp_err_t wifi_config_load_from_nvs(char *ssid, char *password);
static esp_err_t wifi_config_save_to_nvs(const char *ssid, const char *password);
static void wifi_config_set_defaults(char *ssid, char *password);

// WiFi event handler
static void event_handler(void* arg, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi disconnected, trying to reconnect...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// HTTP event handler
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            // Handle response data
            break;
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "HTTP Client Error");
            break;
        default:
            break;
    }
    return ESP_OK;
}

// Button task
static void button_task(void *pvParameter)
{
    bool last_button_state = false;
    
    while(1) {
        bool button_state = gpio_get_level(BUTTON_PIN);
        
        if (button_state != last_button_state && button_state && 
            (esp_timer_get_time() - last_button_press > debounce_delay)) {
            
            last_button_press = esp_timer_get_time();
            display_active = !display_active;
            
            if (display_active) {
                ESP_LOGI(TAG, "Program started");
                gpio_set_level(LED_PIN, 1);
                fetch_bitcoin_data();
                last_fetch_time = esp_timer_get_time();
            } else {
                ESP_LOGI(TAG, "Program stopped");
                gpio_set_level(LED_PIN, 0);
                display_standby();
            }
        }
        
        last_button_state = button_state;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Main task
static void main_task(void *pvParameter)
{
    while(1) {
        if (display_active && (esp_timer_get_time() - last_fetch_time >= fetch_cooldown) && !fetch_in_progress) {
            fetch_bitcoin_data();
            last_fetch_time = esp_timer_get_time();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Bitcoin Price Fetcher...");
    
    #if BREADBOARD_TEST_MODE
    ESP_LOGI(TAG, "BREADBOARD TEST MODE ENABLED - Check connections!");
    #endif
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize GPIO
    gpio_init();
    
    // Initialize I2C
    i2c_master_init();
    
    // Initialize display
    display_init();
    
    // Show welcome screen
    show_welcome_screen();
    
    #if BREADBOARD_TEST_MODE
    // In test mode, wait a bit to check display
    ESP_LOGI(TAG, "Testing display for 3 seconds...");
    vTaskDelay(pdMS_TO_TICKS(3000));
    #endif
    
    // Initialize WiFi
    wifi_init_sta();
    
    // Wait for WiFi connection
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    
    // Initialize HTTP client
    esp_http_client_config_t config = {
        .url = API_URL,
        .event_handler = http_event_handler,
        .timeout_ms = 10000,
    };
    http_client = esp_http_client_init(&config);
    
    // Create tasks
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
    xTaskCreate(main_task, "main_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "System ready! Press button to start program.");
}

// GPIO initialization
static void gpio_init(void)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    
    gpio_set_level(LED_PIN, 0);
}

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

// Display initialization
static void display_init(void)
{
    // SSD1306 initialization sequence
    ssd1306_command(0xAE); // Display off
    ssd1306_command(0xD5); // Set display clock
    ssd1306_command(0x80);
    ssd1306_command(0xA8); // Set multiplex ratio
    ssd1306_command(0x3F);
    ssd1306_command(0xD3); // Set display offset
    ssd1306_command(0x00);
    ssd1306_command(0x40); // Set start line
    ssd1306_command(0x8D); // Charge pump
    ssd1306_command(0x14);
    ssd1306_command(0x20); // Memory mode
    ssd1306_command(0x00);
    ssd1306_command(0xA1); // Segment remap
    ssd1306_command(0xC8); // COM scan direction
    ssd1306_command(0xDA); // COM pins
    ssd1306_command(0x12);
    ssd1306_command(0x81); // Contrast
    ssd1306_command(0xCF);
    ssd1306_command(0xD9); // Pre-charge
    ssd1306_command(0xF1);
    ssd1306_command(0xDB); // VCOM detect
    ssd1306_command(0x40);
    ssd1306_command(0xA4); // Display all on resume
    ssd1306_command(0xA6); // Normal display
    ssd1306_command(0xAF); // Display on
}

// SSD1306 command
static void ssd1306_command(uint8_t cmd)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SCREEN_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, 0x00, true); // Command mode
    i2c_master_write_byte(cmd_handle, cmd, true);
    i2c_master_stop(cmd_handle);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);
}

// SSD1306 data
static void ssd1306_data(uint8_t data)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SCREEN_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, 0x40, true); // Data mode
    i2c_master_write_byte(cmd_handle, data, true);
    i2c_master_stop(cmd_handle);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);
}

// Clear display
static void ssd1306_clear(void)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT / 8; i++) {
        ssd1306_data(0x00);
    }
}

// Display buffer
static void ssd1306_display(void)
{
    // This is a simplified version - in a real implementation you'd use a buffer
    // For now, we'll just clear and redraw
}

// Draw bitmap
static void ssd1306_draw_bitmap(int x, int y, const unsigned char *bitmap, int w, int h)
{
    // Simplified bitmap drawing - in a real implementation you'd need proper buffer management
    // This is a placeholder for the actual implementation
}

// Show welcome screen
static void show_welcome_screen(void)
{
    ssd1306_clear();
    // In a real implementation, you'd draw text and graphics here
    ESP_LOGI(TAG, "Welcome Screen Displayed");
}

// Display bitcoin data
static void display_bitcoin_data(const char *price, double change_24h)
{
    ssd1306_clear();
    // Draw bitcoin icon and price data
    ESP_LOGI(TAG, "Bitcoin Price: $%s, 24h Change: %.1f%%", price, change_24h);
}

// Display error
static void display_error(const char *error_msg)
{
    ssd1306_clear();
    ESP_LOGE(TAG, "Display Error: %s", error_msg);
    gpio_set_level(LED_PIN, 0);
}

// Display message
static void display_message(const char *title, const char *message)
{
    ssd1306_clear();
    ESP_LOGI(TAG, "Display Message - Title: %s, Message: %s", title, message);
}

// Display standby
static void display_standby(void)
{
    ssd1306_clear();
    ESP_LOGI(TAG, "Display Standby");
}

// Print center (placeholder)
static void print_center(const char *buf, int x, int y)
{
    // This would handle text positioning in a real implementation
}

// Fetch bitcoin data
static void fetch_bitcoin_data(void)
{
    if (xEventGroupGetBits(wifi_event_group) & WIFI_CONNECTED_BIT) {
        fetch_in_progress = true;
        
        ESP_LOGI(TAG, "Fetching Bitcoin data from CoinGecko...");
        
        // Set URL
        esp_http_client_set_url(http_client, API_URL);
        
        // Add headers
        esp_http_client_set_header(http_client, "User-Agent", "ESP32-Bitcoin-Fetcher/1.0");
        
        // Perform request
        esp_err_t err = esp_http_client_perform(http_client);
        
        if (err == ESP_OK) {
            int status_code = esp_http_client_get_status_code(http_client);
            int content_length = esp_http_client_get_content_length(http_client);
            
            ESP_LOGI(TAG, "HTTP Status = %d, content_length = %d", status_code, content_length);
            
            if (status_code == 200) {
                // Parse response
                // In a real implementation, you'd parse the JSON response here
                ESP_LOGI(TAG, "Bitcoin data fetched successfully");
            } else {
                ESP_LOGE(TAG, "HTTP request failed with status %d", status_code);
                display_error("HTTP Error");
            }
        } else {
            ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
            display_error("Request Failed");
        }
        
        fetch_in_progress = false;
    } else {
        display_error("WiFi Disconnected");
    }
}

// WiFi initialization
static void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_got_ip));
    
    // Load WiFi credentials from NVS or use defaults
    char ssid[33] = {0};  // 32 chars + null terminator
    char password[65] = {0};  // 64 chars + null terminator
    
    if (wifi_config_load_from_nvs(ssid, password) != ESP_OK) {
        wifi_config_set_defaults(ssid, password);
        ESP_LOGW(TAG, "Using default WiFi credentials. Configure via NVS for production.");
    }
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    
    // Copy credentials to WiFi config (safe string copy)
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WiFi initialized with SSID: %s", ssid);
}

// WiFi configuration functions

// Load WiFi credentials from NVS
static esp_err_t wifi_config_load_from_nvs(char *ssid, char *password)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Open NVS namespace
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS namespace not found, using defaults");
        return ESP_ERR_NOT_FOUND;
    }
    
    // Load SSID
    size_t ssid_len = 32;
    err = nvs_get_str(nvs_handle, NVS_KEY_SSID, ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "SSID not found in NVS, using default");
        nvs_close(nvs_handle);
        return ESP_ERR_NOT_FOUND;
    }
    
    // Load password
    size_t pass_len = 64;
    err = nvs_get_str(nvs_handle, NVS_KEY_PASS, password, &pass_len);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Password not found in NVS, using default");
        nvs_close(nvs_handle);
        return ESP_ERR_NOT_FOUND;
    }
    
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "WiFi credentials loaded from NVS");
    return ESP_OK;
}

// Save WiFi credentials to NVS
static esp_err_t wifi_config_save_to_nvs(const char *ssid, const char *password)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Open NVS namespace for writing
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return err;
    }
    
    // Save SSID
    err = nvs_set_str(nvs_handle, NVS_KEY_SSID, ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save SSID: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }
    
    // Save password
    err = nvs_set_str(nvs_handle, NVS_KEY_PASS, password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save password: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }
    
    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }
    
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "WiFi credentials saved to NVS");
    return ESP_OK;
}

// Set default WiFi credentials
static void wifi_config_set_defaults(char *ssid, char *password)
{
    strcpy(ssid, WIFI_SSID_DEFAULT);
    strcpy(password, WIFI_PASS_DEFAULT);
    ESP_LOGI(TAG, "Using default WiFi credentials");
} 