connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo 210279539596A" && level==0} -index 1
fpga -file D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application/_ide/bitstream/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/system_wrapper_platform_0_1/export/system_wrapper_platform_0_1/hw/system_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow D:/BME/Doktori/Oktatas/2020F2/SoCLabor2/KezdetiMeres/firmware/application/No_optimization/application.elf
configparams force-mem-access 0
bpadd -addr &main
