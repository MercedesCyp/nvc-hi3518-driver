#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "../GlobalParameter.h"
#include "app_def.h"

// extern int gfMsgCenta_DetectDriver(void);
// extern void gShow_ClrShowArea(void);
// extern int gfResolv_UserInput( mCapModule *iMod[] );

extern void gfShow_CapabilitySection(void);
extern void gfShow_AllModeStatus(void);

extern mCapModule *gLinkModule[14];
extern void (*gfMsgCent_Thread1Show)(void);

int main(void)
{
	gfMsgCent_Thread1Show = NULL;
	fcntl(0, F_SETFL, 0);
	if (gfMsgCenta_DetectEnv()) {
		return 0;
	}
	if (gfMsgCenta_DetectDriver()) {
		return 0;
	}
	gfMsgCent_Thread1Show = gfShow_CapabilitySection;
	DF_DCA_FrameUpdate_Stop;
	DF_DCA_FrameUpdate_IncOnce;
	while (1) {
		int tRet;
		tRet = gfResolv_UserInput(gLinkModule);
		if (tRet == -1) {
			system(DCA_FILE_README);

		} else if (tRet == -3) {
			gfMsgCent_Thread1Show = gfShow_AllModeStatus;
			DF_DCA_FrameUpdate_RUN;
			gMsgCentStatus |= DCA_MsgCent_MOD_SHOW;
			while (getchar() != '\n') ;
			gfMsgCent_Thread1Show = gfShow_CapabilitySection;
			gMsgCentStatus &= ~DCA_MsgCent_MOD_SHOW;

		} else if (tRet == -2) {
			break;
		} else if (tRet > 0) {
			if (gLinkModule[tRet - 1]->afModFun()) {
				break;
			}
			gfMsgCent_Thread1Show = gfShow_CapabilitySection;
		}
		DF_DCA_FrameUpdate_IncOnce;
		sleep(1);
	}
	gfMsgCenta_CloseDriver();
	return 0;
}
