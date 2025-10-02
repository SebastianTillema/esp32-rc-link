idf_component_register(
    SRCS
        "receiver/receiver.c"
        "receiver/pwm.c"
        "transmitter/transmitter.c"
        "common/comm.c"
    INCLUDE_DIRS
        "receiver"
        "common"
    PRIV_REQUIRES
        driver
        esp_wifi
        esp_netif
        nvs_flash
)

set(DEFAULT_APP "receiver" CACHE STRING "Choose app: transmitter or receiver")
if(DEFAULT_APP STREQUAL "receiver")
    set_source_files_properties(transmitter/transmitter.c PROPERTIES HEADER_FILE_ONLY TRUE)
elseif(DEFAULT_APP STREQUAL "transmitter")
    set_source_files_properties(receiver/receiver.c receiver/pwm.c PROPERTIES HEADER_FILE_ONLY TRUE)
endif()
