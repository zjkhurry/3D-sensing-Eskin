# invoke SourceDir generated makefile for app_ble.pem3
app_ble.pem3: .libraries,app_ble.pem3
.libraries,app_ble.pem3: package/cfg/app_ble_pem3.xdl
	$(MAKE) -f D:\BLE\test\smallest\GUA_CC2640R2F\MGC3130_cc2640r2f_4x4_app\TOOLS/src/makefile.libs

clean::
	$(MAKE) -f D:\BLE\test\smallest\GUA_CC2640R2F\MGC3130_cc2640r2f_4x4_app\TOOLS/src/makefile.libs clean

