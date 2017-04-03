#include "DIV_main.h"


//  -------------------------------> macro
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
// #define Chip 	chip_type
// #define Pro 	dev_type


//  -------------------------------> Local variable
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
static char *Chip=NULL;
static char *Pro=NULL;
static char *Pro_Cmd=NULL;

module_param(Chip,charp,S_IRUGO);
module_param(Pro,charp,S_IRUGO);
module_param(Pro_Cmd,charp,S_IRUGO);


//  -------------------------------> Function
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--

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
static  int ___init(void)
{
	int32 theRetVal;
	
	if((Pro==NULL)||(Chip==NULL)){
		NVCPrint("Please  use command with similar format to the next line \r\nto instruct me which one I should install !\r\n");
		/*NVC_Print("<insmod NV_Driver.ko Chip=xxxxx Pro=xxx>\r\n");*/
		/*insmod NV_Driver.ko Chip=3518C Pro=D04*/
		
		NVCPrint("<insmod NV_Driver.ko chip_type=xxxxx dev_type=xxx>\r\n");
		/*insmod NV_Driver.ko chip_type=3518C dev_type=D04*/
		
		NVCPrint("xxxxx(3518C,3518E) xxx(D01,D03,D04,D11)>\r\n");
		return -1;
	}
	theRetVal = gfIsTheInputNumExist(Chip,Pro,Pro_Cmd);
	if(theRetVal&DC_DrivNumErrMask){
		NVCPrint("The product you typed in is not exist!");
		return -1;
	}else{
		gInsmodProType =  theRetVal;
		//gInsmodProType = theRetVal&~(DC_DrivNumErrMask);
	}
	
	theRetVal = gfInit_NV_Driver();
	if(theRetVal<0)
		return -1;
	
	NVCPrint("Compile Time = %s %s\n", __DATE__, __TIME__);
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
static void ___exit(void)
{
	if((Pro!=NULL)&&(Chip!=NULL)){
		gfUninit_NV_Driver();
	}else{
		NVCPrint("The kernel may be happened something wrong!\r\n");
	}
	NVCPrint("Compile Time = %s %s\n", __DATE__, __TIME__);
	Pro = NULL;
	Chip = NULL;
	Pro_Cmd = NULL;
	gInsmodProType = 0;
}



//  -------------------------------> DRVSELF DIFINITION
//  --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**-- --**--
module_init(___init);
module_exit(___exit);

MODULE_AUTHOR("Build by maj. 2015-2-10");
MODULE_LICENSE("GPL");
