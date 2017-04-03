#include "GlobalParameter.h"
#include "LinuxAPI.h"
#include "Pro/ProConfig.h"

#include <linux/types.h>
#include <linux/miscdevice.h>

//  -------------------------------> Local variable
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
static char *Chip=NULL;
static char *Pro=NULL;
static char *Pro_Cmd=NULL;

module_param(Chip,		charp,	S_IRUGO);
module_param(Pro,		charp,	S_IRUGO);
module_param(Pro_Cmd,	charp,	S_IRUGO);


//  -------------------------------> Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
extern int32 	gfIsTheInputNumExist(uint8 *iChip,uint8 *iPro,uint8 *iSubCmd );
extern int 		gfINIT_NV_LoadAndConfig(void);
extern void		gfUNINIT_NV_LoadAndConfig(void);

//--------------------------------------------------------------->
// 	Function NAME:
// 		static  int ___init(void)
//	
// 	Param:
// 		void
// 	
// 	Return:
// 		0	Success
// 		-1	Register Driver error
// 	
//--------------------------------------------------------------->
static  int ___onStart(void)
{
	if((Pro==NULL)||(Chip==NULL)){
		NVCPrint("Please  use command with similar format to the next line \r\nto instruct me which one I should install !");
		/*NVCPrint("<insmod NV_Driver.ko Chip=xxxxx Pro=xxx>\r\n");*/
		/*insmod NV_Driver.ko Chip=3518C Pro=D04*/
		NVCPrint("<insmod NV_Driver.ko Chip=xxxxx Pro=xxx>");
		
		NVCPrint("<xxxxx(3518C,3518E) xxx(D01,D03,D04 ... )>");
		return -1;
	}
	if( gfIsTheInputNumExist(Chip,Pro,Pro_Cmd) ){
	// if( gfIsTheInputNumExist(Chip,Pro) ){
		NVCPrint("The product you typed in is not exist!");
		Pro = NULL;
		Chip = NULL;
		Pro_Cmd = NULL;
		return -1;
	}else{
		// execute and loading corresponding drv
		 if( gfINIT_NV_LoadAndConfig() ){
			NVCPrint("error!  cant initial device!");
			return -1;
		}	
	}
	
	NVCPrint("Compile Time = %s %s", __DATE__, __TIME__);
	// return -1;
	return 0;
}


//--------------------------------------------------------------->
// 	Function NAME:
// 		static  int ___init(void)
//
// 	Param:
// 		void
// 	
// 	Return:
// 		void
// 	
//--------------------------------------------------------------->
static void ___onStop(void)
{
	if((Pro!=NULL)&&(Chip!=NULL)){
		gfUNINIT_NV_LoadAndConfig();
	}else{
		NVCPrint("The kernel may be happened something wrong!");
	}
	NVCPrint("Compile Time = %s %s\n", __DATE__, __TIME__);
	
	Pro = NULL;
	Chip = NULL;
	Pro_Cmd = NULL;
}



//  -------------------------------> DRVSELF DIFINITION
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
module_init(___onStart);
module_exit(___onStop);

MODULE_AUTHOR("Build by maj. 2015-2-10");
MODULE_LICENSE("GPL");
