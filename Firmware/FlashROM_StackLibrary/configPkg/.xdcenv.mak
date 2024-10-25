#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = D:/ti/ccs1200/ccs/ccs_base;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/source;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack
override XDCROOT = D:/ti/xdctools_3_62_00_08_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = D:/ti/ccs1200/ccs/ccs_base;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/source;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/kernel/tirtos/packages;D:/BLE/test/smallest/GUA_CC2640R2F_SDK/simplelink_cc2640r2_sdk_1_40_00_45/source/ti/ble5stack;D:/ti/xdctools_3_62_00_08_core/packages;..
HOSTOS = Windows
endif
