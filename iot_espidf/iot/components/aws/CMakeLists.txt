set(COMPONENT_SRCS
	"aws.c"
	"shadow_demo_main.c"
	"shadow_demo_helpers.c"
	)

set(COMPONENT_ADD_INCLUDEDIRS
	"."
	"${CMAKE_CURRENT_LIST_DIR}"
	"${PROJECT_DIR}/../../libraries/common/logging/"
	"../bme680/"
	"../i2c/"
	)

idf_component_register(SRCS "${COMPONENT_SRCS}"
					   INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
					  REQUIRES driver coreMQTT esp_netif protocol_examples_common backoffAlgorithm posix_compat Device-Shadow-for-AWS-IoT-embedded-sdk coreJSON)
