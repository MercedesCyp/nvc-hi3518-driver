#include "HAL.h"
#include "HI3518_HAL.h"


static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id);
static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id);
static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id);



static void sfPin_SetDirection( void *ipPinInfo , uint8 iInOut ){
	mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo;
	// DF_Set_GPIO_MUXx( tPinTool->aMuxVal, tPinTool->aMux );
	if( iInOut&DC_HAL_PIN_SetIN ){
        // DF_Clr_GPIO_DIRx(tPinTool->aGroup,tPinTool->aBit,);
		DC_PIN_SET_IO_IN(tPinTool->aGroup,tPinTool->aBit,tPinTool->aMuxVal, tPinTool->aMux);
	}else if( iInOut&DC_HAL_PIN_SetOUT ){
		DC_PIN_SET_IO_OUT(tPinTool->aGroup,tPinTool->aBit,tPinTool->aMuxVal, tPinTool->aMux);
		// DF_Set_GPIO_DIRx(tPinTool->aGroup,tPinTool->aBit);
	}else{
		return;
	}
}


static void sfPin_SetExport( void *ipPinInfo, uint8 iHighLow){
	mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo;
	if( iHighLow&DC_HAL_PIN_SetLOW ){
		DF_Clr_GPIO_Outx(tPinTool->aGroup,tPinTool->aBit);
	}else if( iHighLow&DC_HAL_PIN_SetHIGH ){
		DF_Set_GPIO_Outx(tPinTool->aGroup,tPinTool->aBit);
	}else if( iHighLow&DC_HAL_PIN_SetSWITCH ){
        DF_Switch_GPIO_Outx(tPinTool->aGroup,tPinTool->aBit);
    }
	
}

static uint32 sPin_GetValue( void *ipPinInfo ){
	mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo;
	return (uint32)( (DF_Get_GPIO_INx(tPinTool->aGroup,tPinTool->aBit))?0x01:0x00 );
}



static uint32 sfPin_WhichGrp( void *ipPinInfo ){
    mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo;
	if(\
	  ( tPinTool->aGroup == 0 )\
	||( tPinTool->aGroup == 1 )\
	||( tPinTool->aGroup == 2 )\
	||( tPinTool->aGroup == 11 ) ){
		return DC_HAL_PIN_Grp0;
	}else if(\
	  ( tPinTool->aGroup == 3 )\
	||( tPinTool->aGroup == 4 )\
	||( tPinTool->aGroup == 5 )\
	||( tPinTool->aGroup == 10 ) ){
		return DC_HAL_PIN_Grp1;
	}else{ 
        return DC_HAL_PIN_Grp2;
	}
}

typedef struct{
	void (*aHandler)(void);
	void *aNext;
}mPINInt_Ifo;

static mPINInt_Ifo *sPinIntLink[3] = {NULL,NULL,NULL};


static uint32 sPin_RegistInterrupt( uint32 iGrp, void (*iHandler)(void) ){
    
	if( sPinIntLink[iGrp] == NULL ){
		sPinIntLink[iGrp] = (mPINInt_Ifo*)kmalloc(sizeof(mPINInt_Ifo),GFP_ATOMIC);
		
		sPinIntLink[iGrp]->aHandler = iHandler;
		sPinIntLink[iGrp]->aNext 	= NULL;
		
		if( iGrp == DC_HAL_PIN_Grp0 ){
			if(request_irq(	IRQ_GPIO29,\
							intf_GPIO_Grp1Function,\
							IRQF_SHARED,\
							"NV_Drv_PinGrp1",\
							&sPinIntLink[0]))
				return DC_HAL_PIN_RetIntErr;
		}else if( iGrp == DC_HAL_PIN_Grp1 ){
			
			if(request_irq(	IRQ_GPIO30,\
							intf_GPIO_Grp2Function,\
							IRQF_SHARED,\
							"NV_Drv_PinGrp2",\
							&sPinIntLink[1]))
				return DC_HAL_PIN_RetIntErr;
		}else if( iGrp == DC_HAL_PIN_Grp2 ){
			if(request_irq(	IRQ_GPIO31,\
							intf_GPIO_Grp3Function,\
							IRQF_SHARED,\
							"NV_Drv_PinGrp3",\
							&sPinIntLink[2]))
				return DC_HAL_PIN_RetIntErr;
		}

	}else{
		mPINInt_Ifo *tLinkTools	= sPinIntLink[iGrp] ;
		mPINInt_Ifo *tLinkTools_2;
		
		for(;;){
			if( tLinkTools->aNext == NULL ){
				break;
			}else{
				tLinkTools 		= tLinkTools->aNext;
			}
		}
		
		tLinkTools_2 			= (mPINInt_Ifo*)kmalloc(sizeof(mPINInt_Ifo),GFP_ATOMIC);
		
		tLinkTools_2->aHandler 	= iHandler;
		tLinkTools_2->aNext 	= NULL;
		
		tLinkTools->aNext 		= tLinkTools_2;
		
		// 初始化 IO 中断
	}
	return DC_HAL_PIN_RetSuccess;
}

static void sfPin_ClrIntConfig(void){
    uint8 _i;
    uint8 irq_num[3]={IRQ_GPIO29,IRQ_GPIO30,IRQ_GPIO31};
    mPINInt_Ifo *tLinkTools,*tLinkTools_next;
    
    for( _i=0; _i<3; _i++ ){
        tLinkTools = sPinIntLink[_i];
        if( tLinkTools != NULL ){
            free_irq(irq_num[_i],&sPinIntLink[_i]);
            do{
                tLinkTools_next = tLinkTools->aNext;
                kfree(tLinkTools);
                tLinkTools = tLinkTools_next;
            }while( tLinkTools != NULL );
        }
    }
}

static uint32 sfPin_IsIntExist( void *ipPinInfo ){
    mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo; 
    return DC_Get_GPIO_RIS( tPinTool->aGroup, tPinTool->aBit );
}

static void sPin_ConfigInt( void *ipPinInfo , uint8 iCmd ){
	
    mGPIOPinIfo *tPinTool =  (mGPIOPinIfo*)ipPinInfo; 
    if( iCmd&DC_HAL_PIN_IntIN ){
        DC_PIN_SET_IO_IN(tPinTool->aGroup,tPinTool->aBit,tPinTool->aMuxVal, tPinTool->aMux);
	// DF_Set_GPIO_MUXx(tPinTool->aMuxVal,tPinTool->aMux);	// Setting multi function
	// DF_Clr_GPIO_DIRx(tPinTool->aGroup,tPinTool->aBit);	// Setting IO dirction INPUT 
    }
	
	if( iCmd == DC_HAL_PIN_IntHighLevel ){
		DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Set_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IEV(tPinTool->aGroup));
	
	}else if( iCmd == DC_HAL_PIN_IntLowLevel ){
		DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IEV(tPinTool->aGroup));
	
	}else if( iCmd == DC_HAL_PIN_IntRiseEdge ){
		DF_Set_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Set_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IEV(tPinTool->aGroup));
        DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IBE(tPinTool->aGroup));
	
	}else if( iCmd == DC_HAL_PIN_IntFallEdge ){
		DF_Set_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IS(tPinTool->aGroup));
		DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IEV(tPinTool->aGroup));
        DF_Clr_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IBE(tPinTool->aGroup));
        
	}
	
    if( iCmd&DC_HAL_PIN_IntClrState ){
    // Clear Int status
        DC_Clr_GPIO_INT_Sta(tPinTool->aGroup,tPinTool->aBit);
    }
	
    if( iCmd&DC_HAL_PIN_IntEnable ){
	// Enable interrupt
        DF_Set_GPIO_ADDx(tPinTool->aGroup,tPinTool->aBit,HAL_GPIOx_IE(tPinTool->aGroup));
    }
}


static irqreturn_t intf_GPIO_Grp1Function(int irq, void *dev_id){
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[0];
	
	for(;tPinTool_1 != NULL;){
        if( tPinTool_1->aHandler != NULL )
            tPinTool_1->aHandler();
		tPinTool_1 = tPinTool_1->aNext;
	}
	return 0;
}


static irqreturn_t intf_GPIO_Grp2Function(int irq, void *dev_id){
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[1];
	
	for(;tPinTool_1 != NULL;){
        if( tPinTool_1->aHandler != NULL )
            tPinTool_1->aHandler();
		tPinTool_1 = tPinTool_1->aNext;
	}
	return 0;
}

static irqreturn_t intf_GPIO_Grp3Function(int irq, void *dev_id){
	mPINInt_Ifo *tPinTool_1 = sPinIntLink[2];
	
	for(;tPinTool_1 != NULL;){
        if( tPinTool_1->aHandler != NULL )
            tPinTool_1->aHandler();
		tPinTool_1 = tPinTool_1->aNext;
	}
	return 0;
}





mClass_Pin const gcoClass_Pin = {
	.prfSetDirection        = sfPin_SetDirection,
	.prfSetExport           = sfPin_SetExport,
	.prfGetPinValue         = sPin_GetValue,
    .prfWhichGrp            = sfPin_WhichGrp,
	.prfPin_RegistInt       = sPin_RegistInterrupt,
    .prfClrConfig           = sfPin_ClrIntConfig,
    .prfPin_ConfigInt       = sPin_ConfigInt,
    .prfIsIntStateExist     = sfPin_IsIntExist
};
