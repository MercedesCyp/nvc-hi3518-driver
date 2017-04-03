all:
	cd drv/button;make
#	cd drv/irc;make
	cd drv/night_light;make
	cd drv/pt;make
	cd drv/sensor_select;make
	cd app/sensor_init;make
#	cd drv/sensor_switch;make	
	cd drv/state_led;make
	cd drv/tmp_fan/AD_NTC;make
	cd drv/tmp_fan/APP;make
	cd drv/tmp_fan/DA_PWM;make
	cd drv/tmp_fan/SHIFT_LENS;make

clean:
	cd drv/button;make clean
#	cd drv/irc;make clean
	cd drv/night_light;make clean
	cd drv/pt;make clean
	cd drv/sensor_select;make clean
	cd app/sensor_init;make clean
#	cd drv/sensor_switch;make clean
	cd drv/state_led;make clean
	cd drv/tmp_fan/AD_NTC;make clean
	cd drv/tmp_fan/APP;make clean
	cd drv/tmp_fan/DA_PWM;make clean
	cd drv/tmp_fan/SHIFT_LENS;make clean

dist_clean:clean
	rm -rf build/*	
