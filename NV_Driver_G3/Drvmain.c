/*
Coder:      aojie.meng
Date:       2015-8-15

Abstract:
    驱动程序装在入口，提供驱动在 insmod rmmod 时候的 linux 内核回调函数，在回调
    函数中会根据驱动需求进行申请资源，释放资源和初始化模块之类的工作。
*/
//==============================================================================
// C
// Linux
#include <linux/types.h>
#include <linux/miscdevice.h>
// local
#include "GlobalParameter.h"
#include "LinuxAPI.h"
// remote
#include "Tool/String.h"
#include "Pro/ProConfig.h"

//==============================================================================
//extern
extern int32 gfIsTheInputNumExist(uint8 *iChip, uint8 *iPro, uint8 *iSubCmd);
extern int gfINIT_NV_LoadAndConfig(void);
extern void gfUNINIT_NV_LoadAndConfig(void);
//local
//global

//==============================================================================
//extern
//local
static char *Chip = NULL;
static char *Pro = NULL;
static char *Pro_Cmd = NULL;
//global
//init
module_param(Chip, charp, S_IRUGO);
module_param(Pro, charp, S_IRUGO);
module_param(Pro_Cmd, charp, S_IRUGO);

//==============================================================================
//Global

//------------------------------------------------------------------------------
//Local
//---------- ---------- ---------- ----------
/*  static  int ___init(void)
@introduction:
    NULL
@parameter:
    void
@return:
    0	Success
    -1	Register Driver error
*/
static int ___onStart(void)
{
	int32 _ret = -1;
	if ((Pro == NULL) || (Chip == NULL)) {
		NVCPrint
		("Please use command with similar format to the next line \r\nto instruct me which one I should install !");
		/*NVCPrint("<insmod NV_Driver.ko Chip=xxxxx Pro=xxx>\r\n"); */
		/*insmod NV_Driver.ko Chip=3518C Pro=D04 */
		NVCPrint("<insmod NV_Driver.ko Chip=xxxxx Pro=xxx>");

		NVCPrint
		("<xxxxx(3518C,3518E,3518EV200) xxx(D01,D03,D04 ... )>");
		return -1;
	}

	if ((_ret = gfIsTheInputNumExist(Chip, Pro, Pro_Cmd)) != 0) {
		if (_ret == DC_DrivChipErr) {
			NVCPrint
			("The parameter Chip you typed in is not exist!");
		} else if (_ret == DC_DrivProErr) {
			NVCPrint
			("The parameter Pro you typed in is not exist!");
		} else {
			NVCPrint("The parameter you typed in is not exist!");
		}
		Pro = NULL;
		Chip = NULL;
		Pro_Cmd = NULL;
		return -1;
	} else {
		// execute and loading corresponding drv
		if (gfINIT_NV_LoadAndConfig()) {
			NVCPrint("error! Can't initial device!");
			return -1;
		}
	}

	NVCPrint("Compile Time      = %s %s", __DATE__, __TIME__);
	NVCPrint("Compile Version   = %s", DC_NVDRIV_VERSION);
	return 0;
}

//---------- ---------- ---------- ----------
/*  static void ___onStop(void)
@introduction:
    NULL
@parameter:
    void
@return:
    void
*/
static void ___onStop(void)
{
	if ((Pro != NULL) && (Chip != NULL)) {
		gfUNINIT_NV_LoadAndConfig();
	} else {
		NVCPrint("The kernel may be happened something wrong!");
	}

	NVCPrint("Compile Time      = %s %s", __DATE__, __TIME__);
	NVCPrint("Compile Version   = %s", DC_NVDRIV_VERSION);

	Pro = NULL;
	Chip = NULL;
	Pro_Cmd = NULL;
}

//==============================================================================
//Others
module_init(___onStart);
module_exit(___onStop);

MODULE_AUTHOR("Build by maj. 2015-2-10");
MODULE_LICENSE("GPL");
