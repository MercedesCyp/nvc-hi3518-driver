#ifndef __ProInclude_H
#define __ProInclude_H

#include "../GlobalParameter.h"
#include "../Tool/String.h"
#include "../CTA/CTA.h"
#include <linux/slab.h>


typedef struct{
	uint8	  aNum;
	mArr	 *apCmd;
	void	(*afCmdProcessing)(uint32);
}mSubCmd;

typedef struct{
	mArr		  aPro;
	mArr		  aChip;
	mSubCmd		 *aSubCmd;
	mClass_CTA*	(*afRewriteAndConfig)(void);
	int32		(*afInit)(void);
	int32		(*afUninit)(void);	
}mProInfo;

typedef struct{
	mProInfo	 *apLogedPro;
	mClass_CTA	 *apCTA;
}mClass_Pro;

extern mClass_Pro gClassPro;

#endif
