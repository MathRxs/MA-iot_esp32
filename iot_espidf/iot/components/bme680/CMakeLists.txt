idf_component_register(SRCS  "bme680_platform.c" "bme680.c" "bme680_task.c"
                       INCLUDE_DIRS  "../i2c" "."
                       REQUIRES driver)