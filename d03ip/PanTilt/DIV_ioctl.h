#ifndef __DIV_IOCTL_H
#define __DIV_IOCTL_H

//#define IOCMD_NV_TYPE	'x'
#define IOCMD_NV_TYPE	'p'

//PanTilt start 10 end 69//Run one step to left
#define IOCMD_PT_Min				10
#define IOCMD_PT_Max				69

// PT 	PanTilt
// H 	Horizon
// V 	Vertical
// R 	Right
// L 	Lift
// Up	Up
// Dn 	DOWN
// Clk	Clockwise
// Aclk	Anticlockwise
// AS	one step
// SS	Some Step
// Dgr	degree
// DgrS	degres
#define IOCMD_PT_HClk_AS			_IO(IOCMD_NV_TYPE, 10)
#define IOCMD_PT_HAclk_AS			_IO(IOCMD_NV_TYPE, 11)
#define IOCMD_PT_VUp_AS				_IO(IOCMD_NV_TYPE, 13)
#define IOCMD_PT_VDn_AS				_IO(IOCMD_NV_TYPE, 12) 


#define IOCMD_PT_HClk_Dgr30			_IO(IOCMD_NV_TYPE, 55) 
#define IOCMD_PT_HAclk_Dgr30		_IO(IOCMD_NV_TYPE, 56)
#define IOCMD_PT_VUp_Dgr15			_IO(IOCMD_NV_TYPE, 58)
#define IOCMD_PT_VDn_Dgr15			_IO(IOCMD_NV_TYPE, 57)

#define IOCMD_PT_HClk_SS			_IOW(IOCMD_NV_TYPE, 62, int )
#define IOCMD_PT_HAclk_SS			_IOW(IOCMD_NV_TYPE, 63, int )
#define IOCMD_PT_VUp_SS				_IOW(IOCMD_NV_TYPE, 65, int )
#define IOCMD_PT_VDn_SS				_IOW(IOCMD_NV_TYPE, 64, int )


#define IOCMD_PT_HClk_DgrS			_IOW(IOCMD_NV_TYPE, 66, int ) 
#define IOCMD_PT_HAclk_DgrS			_IOW(IOCMD_NV_TYPE, 67, int )
#define IOCMD_PT_VUp_DgrS			_IOW(IOCMD_NV_TYPE, 69, int )
#define IOCMD_PT_VDn_DgrS			_IOW(IOCMD_NV_TYPE, 68, int )




/*
#define TRACK_01                    _IO(PT_IOC_MAGIC, 59) 
#define TRACK_02                    _IO(PT_IOC_MAGIC, 60) 
#define TRACK_03                    _IO(PT_IOC_MAGIC, 61)
*/
















#endif