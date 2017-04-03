all:
	cd d01ip;make
	cd d11ip;make
clean:
	cd d01ip;make clean
	cd d11ip;make clean

dist_clean:
	cd d01ip;make dist_clean
	cd d11ip;make dist_clean	
