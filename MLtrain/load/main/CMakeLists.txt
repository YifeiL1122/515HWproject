idf_component_register(
    SRCS 
        "main.c"
        "dl_detect.c"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_dl
        esp_camera
        esp_http_server
        esp_wifi
        nvs_flash
        driver
        esp_http_client
        esp_https_ota
        protocol_examples_common
)
