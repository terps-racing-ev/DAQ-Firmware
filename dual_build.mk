# Dual bank wrapper for STM32Make.make
# Produces daq_firmware_a.bin and daq_firmware_b.bin in one build invocation.

BASE_TARGET ?= daq_firmware
INNER_MAKE ?= STM32Make.make

.PHONY: all bank_a bank_b clean

all: bank_a bank_b

bank_a:
	$(MAKE) -f $(INNER_MAKE) \
		TARGET=$(BASE_TARGET)_a \
		LDSCRIPT=STM32L432XX_APP_BANK_A.ld \
		C_DEFS="-DSTM32L432xx -DUSE_HAL_DRIVER -DVECT_TAB_OFFSET=0x8000 -DUSER_VECT_TAB_ADDRESS"

bank_b:
	$(MAKE) -f $(INNER_MAKE) \
		TARGET=$(BASE_TARGET)_b \
		LDSCRIPT=STM32L432XX_APP_BANK_B.ld \
		C_DEFS="-DSTM32L432xx -DUSE_HAL_DRIVER -DVECT_TAB_OFFSET=0x22000 -DUSER_VECT_TAB_ADDRESS"

clean:
	$(MAKE) -f $(INNER_MAKE) clean
