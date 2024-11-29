idf_component_register(SRCS "bme680.c" 
                       INCLUDE_DIRS "." "../i2c/"
                       REQUIRES driver)