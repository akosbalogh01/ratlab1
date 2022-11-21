#
# This file was created by Xilinx Vitis tool.
#


#
# tools
#

RM = rm -f
RMDIR = rm -rf

#
# variables
#

SDCARD_GEN = sdcard_gen
GENERATE_BIF = ::scw::generate_bif

XPFM_PATH = D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/system_wrapper_platform_0_1/export/system_wrapper_platform_0_1/system_wrapper_platform_0_1.xpfm
SYS_CONFIG = system_wrapper_platform_0_1
DOMAINS = bsp_domain
BIF_PATH = D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application_system/Debug/system.bif

BITSTREAM_PATH = D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application/_ide/bitstream/system_wrapper.bit
SD_FILES = --elf D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application/No_optimization/application.elf,ps7_cortexa9_0

#
# primary build targets
#

all: sd_card

sd_card:
	-@echo Generating bif file for the system project
	generate_system_bif.bat 59377 ${XPFM_PATH} ${DOMAINS} ${BIF_PATH}
	${SDCARD_GEN} --xpfm ${XPFM_PATH} --sys_config ${SYS_CONFIG} --bif ${BIF_PATH} --bitstream ${BITSTREAM_PATH} ${SD_FILES}

clean:
	$(RMDIR) ./_sds
	${RMDIR} sd_card
	${RM} system.bif

.PHONY: all sd_card clean


