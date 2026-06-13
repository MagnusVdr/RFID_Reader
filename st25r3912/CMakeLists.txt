idf_component_register(
    SRCS "st25r3912_hal.c" "st25r3912.c"
    INCLUDE_DIRS "."
    REQUIRES driver 
    esp_hw_support 
    esp_driver_spi 
    esp_driver_gpio
    esp_timer
)