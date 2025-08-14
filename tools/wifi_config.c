#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

// WiFi configuration constants
#define NVS_NAMESPACE "wifi_config"
#define NVS_KEY_SSID "ssid"
#define NVS_KEY_PASS "password"

static const char *TAG = "WIFI_CONFIG";

// Function to save WiFi credentials to NVS
esp_err_t wifi_config_save(const char *ssid, const char *password)
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
    ESP_LOGI(TAG, "WiFi credentials saved successfully!");
    ESP_LOGI(TAG, "SSID: %s", ssid);
    ESP_LOGI(TAG, "Password: %s", password);
    return ESP_OK;
}

// Function to clear WiFi credentials from NVS
esp_err_t wifi_config_clear(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Open NVS namespace for writing
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return err;
    }
    
    // Erase the entire namespace
    err = nvs_erase_all(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase NVS namespace: %s", esp_err_to_name(err));
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
    ESP_LOGI(TAG, "WiFi credentials cleared successfully!");
    return ESP_OK;
}

// Function to read WiFi credentials from NVS
esp_err_t wifi_config_read(char *ssid, char *password, size_t ssid_len, size_t pass_len)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Open NVS namespace
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS namespace not found");
        return ESP_ERR_NOT_FOUND;
    }
    
    // Load SSID
    err = nvs_get_str(nvs_handle, NVS_KEY_SSID, ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "SSID not found in NVS");
        nvs_close(nvs_handle);
        return ESP_ERR_NOT_FOUND;
    }
    
    // Load password
    err = nvs_get_str(nvs_handle, NVS_KEY_PASS, password, &pass_len);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Password not found in NVS");
        nvs_close(nvs_handle);
        return ESP_ERR_NOT_FOUND;
    }
    
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "WiFi credentials read successfully!");
    ESP_LOGI(TAG, "SSID: %s", ssid);
    ESP_LOGI(TAG, "Password: %s", password);
    return ESP_OK;
}

// Example usage in main function
void app_main(void)
{
    ESP_LOGI(TAG, "WiFi Configuration Utility");
    ESP_LOGI(TAG, "==========================");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Example: Save WiFi credentials
    const char *my_ssid = "Your_Actual_WiFi_SSID";
    const char *my_password = "Your_Actual_WiFi_Password";
    
    ESP_LOGI(TAG, "Setting WiFi credentials...");
    if (wifi_config_save(my_ssid, my_password) == ESP_OK) {
        ESP_LOGI(TAG, "WiFi credentials saved successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to save WiFi credentials!");
    }
    
    // Example: Read back the credentials
    char read_ssid[33] = {0};
    char read_password[65] = {0};
    
    ESP_LOGI(TAG, "Reading WiFi credentials...");
    if (wifi_config_read(read_ssid, read_password, sizeof(read_ssid), sizeof(read_password)) == ESP_OK) {
        ESP_LOGI(TAG, "Credentials verified successfully!");
    } else {
        ESP_LOGE(TAG, "Failed to read WiFi credentials!");
    }
    
    ESP_LOGI(TAG, "WiFi configuration utility completed!");
}
