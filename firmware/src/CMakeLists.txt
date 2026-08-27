file(GLOB_RECURSE custom_sources "*.c")

idf_component_register(
    SRCS 
        ${custom_sources}
    
    # Point to the root inc folder and all its subdirectories
    INCLUDE_DIRS 
        "../inc"
        "../inc/devices"
        "../inc/peripherals"
        "../inc/network"
        "../inc/app"
    
    REQUIRES 
        driver 
        esp_wifi 
        lwip 
        nvs_flash
        esp_driver_i2c
)