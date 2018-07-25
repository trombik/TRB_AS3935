#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := TRB_AS3935
UNIT_TEST_APP :=	$(IDF_PATH)/tools/unit-test-app
# include unity component for unit testing
EXTRA_COMPONENT_DIRS=	$(UNIT_TEST_APP)/components
SDKCONFIG_DEFAULTS=	 $(UNIT_TEST_APP)/sdkconfig.defaults

CFLAGS := -DGPIO_SDA=21 -DGPIO_SCL=22 -D TRB_AS3935_ESP_IDF -D IRQ_PIN=19
ifndef WITHOUT_ESP_ERR_TO_NAME
CFLAGS += -D HAVE_ESP_ERR_TO_NAME
endif

TEST_COMPONENTS = "TRB_AS3935"

include $(UNIT_TEST_APP)/Makefile
