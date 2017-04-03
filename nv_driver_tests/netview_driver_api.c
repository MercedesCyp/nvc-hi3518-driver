#include "netview_driver_api.h"

#define ECHO_COLOR_NONE     "\033[0m"
#define ECHO_COLOR_WHITE    "\033[1;37m"
#define ECHO_COLOR_YELLOW   "\033[1;33m"
#define ECHO_COLOR_RED      "\033[0;31m"

float g_fDriver_version = 1.0;

union
{
	Nvc_Drv_Message	stDrvMsg;
	char szMsgBuff[1024];
}g_unMsg;

static void nv_error_printout(char *type, char *fmt, char *file, int line, va_list argp)
{
	char szColor[12];

	if (strcasecmp(type, "DEBUG") == 0)
	{
		strcpy(szColor, ECHO_COLOR_NONE);
	}
	else if (strcasecmp(type, "WARN") == 0)
	{
		strcpy(szColor, ECHO_COLOR_YELLOW);
	}
	else if (strcasecmp(type, "ERROR") == 0)
	{
		strcpy(szColor, ECHO_COLOR_RED);
	}
	else
	{
		strcpy(szColor, ECHO_COLOR_NONE);
	}

	//
	char buf[1024];
	struct tm *local;
	char timestr[256];
    struct timeval now;
    gettimeofday(&now, NULL);
    local = localtime(&now.tv_sec);

    strftime(timestr, 256, "%Y-%m-%d %H:%M:%S", local);
    vsnprintf(buf, 1024, fmt, argp);
	printf("%s%s %s: %s "
		"(%s line %d)"
		"\n"ECHO_COLOR_NONE, szColor, timestr, type, buf
		, file, line
		);
}

void nv_debug_message(char *fmt, char *file, int line, ...)
{
	va_list argp;
	va_start(argp, line);
	nv_error_printout("DEBUG", fmt, file, line, argp);
	va_end(argp);
}

void nv_warn_message(char *fmt, char *file, int line, ...)
{
	va_list argp;
	va_start(argp, line);
	nv_error_printout("WARN", fmt, file, line, argp);
	va_end(argp);
}

void nv_error_message(char *fmt, char *file, int line, ...)
{
	va_list argp;

	va_start(argp, line);
	nv_error_printout("ERROR", fmt, file, line, argp);
	va_end(argp);
}

NV_U64 nv_media_get_sys_pts()
{
	struct timespec time1 = {0, 0};
	clock_gettime(CLOCK_REALTIME, &time1);
	return time1.tv_sec*1000000UL + time1.tv_nsec/1000UL;
}

//
int nv_drv_debug_msg_head(char *pBuffer, int nLen)
{
	int i = 0;
	printf("\nRecv Msg header Size %d, Context: ", nLen);
	for (i = 0; i < nLen; i++)
	{
		printf("0x%02x ", pBuffer[i]);
	}
	printf("\n");
	return NV_SUCCESS;
}

int nv_drv_pack_msg_hdr(char *pHdrBuff, int nMsgType, int nMsgLen, int nDevno)
{
	Nvc_Driver_Msg_Hdr_S *pMsgHdr = (Nvc_Driver_Msg_Hdr_S *)pHdrBuff;
	pMsgHdr->u16Magic = NV_DRIVER_MSG_MAGIC_NUM;
	pMsgHdr->u16MsgType = nMsgType;
	pMsgHdr->u16MsgLen = nMsgLen;
	pMsgHdr->u8DevNo = nDevno;
	pMsgHdr->u8ErrCode = 0;
	memset(pMsgHdr->u8Res, 0, sizeof(pMsgHdr->u8Res));
	return sizeof(Nvc_Driver_Msg_Hdr_S);
}

int nv_drv_parse_msg_hdr(char *pMsgBuff, int nMsgLen, int *pErrCode)
{
	Nvc_Driver_Msg_Hdr_S *pMsgHdr = (Nvc_Driver_Msg_Hdr_S *)pMsgBuff;
	if (pMsgHdr->u16Magic != NV_DRIVER_MSG_MAGIC_NUM 
		|| pMsgHdr->u16MsgLen != nMsgLen - sizeof(Nvc_Driver_Msg_Hdr_S))
	{
		return NV_FAILURE;
	}
	
	return NV_SUCCESS;
}

int nv_drv_recv_message(int nFd, char *pMsgBuf, int nBufLen, int nTimeOut)
{
	NV_U64 u64FirPts = 0;
	NV_S32 nSize = 0, nTotSize = 0;

	while(1)
	{
		if ((nSize = read(nFd, pMsgBuf + nTotSize, nBufLen - nTotSize)) == 0)
		{
			NV_ERROR("nv_drv_recv_message, errno: [%d]£¬errmsg: [%s]", errno, strerror(errno));
			return NV_FAILURE;
		}
		
		if (nSize > 0)
		{
			nTotSize += nSize;
#if 1			
			return nTotSize;
#endif
		}
		else
		{
			//if(ECONNRESET == errno)return NV_FAILURE;
			if(EWOULDBLOCK != errno  && errno != EINTR && errno != EAGAIN)
			{
				NV_ERROR("nv_drv_recv_message, errno: [%d]£¬errmsg: [%s]", errno, strerror(errno));
				return NV_FAILURE;
			}
            if (nTimeOut != 0)
    		{
    		    if (u64FirPts == 0)
    				u64FirPts = nv_media_get_sys_pts();
    			else if (nv_media_get_sys_pts() - u64FirPts >= nTimeOut*1000UL)
    				break;

    			usleep(10000);
            }
		}
	}
	if (nTotSize == 0)
	{
		NV_ERROR("nv_drv_recv_message timeout!");
		return NV_FAILURE;
	}
	return nTotSize;
}

int nv_drv_send_message(int nFd, char *pMsgBuf, int nMsgLen)
{
	NV_S32 tlen = 0;
	NV_S32 slen = 0;
	int i = 0;
	
	if(NULL == pMsgBuf)
		return NV_FAILURE;
	do
	{
		//slen = write(nFd, &((char *)pMsgBuf)[tlen], nMsgLen - tlen);
#if DEBUG_FM1288
		printf("\n Write Data Ready! nMsgLen = %d\n", nMsgLen);
#endif
		slen = write(nFd, (pMsgBuf + tlen), nMsgLen - tlen);
#if DEBUG_FM1288
		printf("\n nv_drv_send_message: write slen = %d\n", slen);
#endif
		if (slen <= 0 )
		{
            //tlen = slen;
			NV_ERROR("nv_drv_send_message, errno: [%d]£¬errmsg: [%s]", errno, strerror(errno));
			break;
		}
		else
			tlen = tlen + slen;
	} while(tlen < nMsgLen);
	return tlen;
}

int nv_drv_select_fd(int fd, int time_out)
{
	int maxfd = 0;
	int ret   = 0;
	
	fd_set read_fd, *p_read_fd;
	struct timeval timeO, *p_time_out;

	if (fd < 0)
	{		
		NV_ERROR("fd < 0");
		return -1;
	}
	p_time_out = NULL;
	if (time_out > 0)
	{
    	timeO.tv_sec = time_out / 1000;
    	timeO.tv_usec= (time_out % 1000)*1000;	
		p_time_out = &timeO;
	}

	p_read_fd = &read_fd;
	FD_ZERO(p_read_fd);
	FD_SET(fd, p_read_fd);

	maxfd = fd + 1;

	while (1)
	{
		ret = select(maxfd, p_read_fd, NULL, NULL, p_time_out);
		if (ret < 0 && errno == EINTR)
		{
		    continue;
		}    
		else if (ret < 0)
			return -1;
		else if (ret == 0)
			return 0;
		else
		{ 
			if (FD_ISSET(fd, p_read_fd))
				return 1;
			else
				return 0;
		}
	}	
	return -1;	
}

int nv_drv_wait_message(int fd, int nMsgType, char *pBuffer, int nBufLen, int nTimeOut)
{
    int nTryTimes = 3;
    int nRecvLen;
	NV_U64 u64BegPts = 0;
	Nvc_Driver_Msg_Hdr_S *pMsgHdr = (Nvc_Driver_Msg_Hdr_S *)pBuffer;
    
    if (nTimeOut == 0)
    {
        if ((nRecvLen = nv_drv_recv_message(fd, pBuffer, nBufLen, 0)) < sizeof(Nvc_Driver_Msg_Hdr_S) 
            || pMsgHdr->u16MsgType != nMsgType)
        {
        	NV_ERROR("nv_drv_recv_message FAILURE");
            return NV_FAILURE;
        }
        return nRecvLen;
    }
    
    u64BegPts = nv_media_get_sys_pts();
    do
    {
        if (nv_drv_select_fd(fd, nTimeOut) != 1)
        {        	
        	NV_ERROR("nv_drv_select_fd ");
            return NV_FAILURE;
        }
        if ((nRecvLen = nv_drv_recv_message(fd, pBuffer, nBufLen, 0)) < sizeof(Nvc_Driver_Msg_Hdr_S))
        {        	
        	NV_ERROR("nv_drv_recv_message FAILURE");
			return NV_FAILURE;
        }

        if (pMsgHdr->u16Magic == NV_DRIVER_MSG_MAGIC_NUM && pMsgHdr->u16MsgType == nMsgType)
        {        	
        	return nRecvLen;
        }

        nTimeOut -= ((nv_media_get_sys_pts() - u64BegPts)/1000UL);
    }while(nTimeOut > 0 && nTryTimes-- > 0);
    return NV_FAILURE;
}

int nv_drv_set_attached_msg(int nFd, NV_U32 u32Attached)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	PLGIN_DEBUG("Set Attached Message :[%d]!", u32Attached);
    pDrvMsg->stAttachedMsg.u8Attached = u32Attached == 0 ? 0 : 1;
    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ?  NVC_SET_ATTACHED_DRIVER_MSG_REQ  : NVC_OLD_SET_ATTACHED_DRIVER_MSG_REQ, sizeof(Nvc_Attached_Driver_Msg_S), 0) + sizeof(Nvc_Attached_Driver_Msg_S);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Driver Attached Msg Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_SET_ATTACHED_DRIVER_MSG_RESP : NVC_OLD_SET_ATTACHED_DRIVER_MSG_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Transfer Driver Attached Msg Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Transfer Driver Attached Operate Error :%d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	return NV_SUCCESS;
}

int nv_drv_init_device_info(int nFd, NvcDeviceType_E *pDevType, Nvc_Driver_Cap_Info_S *pCapInfo)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	char *pVerVal = NULL;
    //
	nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_DRIVER_INFO_REQ : NVC_OLD_QUERY_DRIVER_INFO_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Driver Version Msg Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_DRIVER_INFO_RESP : NVC_OLD_QUERY_DRIVER_INFO_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Driver Version Msg Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 2*sizeof(NV_U32))
    {
        NV_ERROR("Get Driver Version Operate Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	if (pDevType != NULL)
	{
		*pDevType = pDrvMsg->stVerInfo.u32DeviceType;
		PLGIN_DEBUG("szVerInfo:[%s], szBuildData :[%s], u32ChipType :[0x%x], u32DeviceType :[0x%x]!", pDrvMsg->stVerInfo.szVerInfo, pDrvMsg->stVerInfo.szBuildData,
			pDrvMsg->stVerInfo.u32ChipType, pDrvMsg->stVerInfo.u32DeviceType);
	}
	if ((pVerVal = strstr(pDrvMsg->stVerInfo.szVerInfo, ":")) != NULL)
	{
		pVerVal++;
		while (*pVerVal == ' ')
			pVerVal++;
		g_fDriver_version = atof(pVerVal);
	}
	else if (isdigit((int)(pDrvMsg->stVerInfo.szVerInfo[0])))
	{
		pVerVal = pDrvMsg->stVerInfo.szVerInfo;
		g_fDriver_version = atof(pVerVal);
	}
	//
	if (pCapInfo != NULL)
	{
		nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_DRIVER_CAPACITY_REQ : NVC_OLD_QUERY_DRIVER_CAPACITY_REQ, 0, 0);
	    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	    {
	        NV_ERROR("Transfer Get Device Ability Msg Error!");
	        return NV_FAILURE;
	    }
	    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_DRIVER_CAPACITY_RESP : NVC_OLD_QUERY_DRIVER_CAPACITY_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
	    {
	        NV_ERROR("Wait Get Device Ability Msg Resp Error!");
	        return NV_FAILURE;
	    }
	    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 2*sizeof(NV_U32))
	    {
	        NV_ERROR("Get Device Ability Operate Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
	        return NV_FAILURE;
	    }
	    memset(pCapInfo, 0, sizeof(Nvc_Driver_Cap_Info_S));
	    nSize = pDrvMsg->stMsgHdr.u16MsgLen > sizeof(Nvc_Driver_Cap_Info_S) ? sizeof(Nvc_Driver_Cap_Info_S) : pDrvMsg->stMsgHdr.u16MsgLen;
	    memcpy(pCapInfo, &pDrvMsg->stCapInfo, nSize);
		PLGIN_DEBUG("u32CapMask:[0x%x], u8ButtonCnt :[%d], u8LedCnt :[%d]!", pDrvMsg->stCapInfo.u32CapMask, pDrvMsg->stCapInfo.u8ButtonCnt, pDrvMsg->stCapInfo.u8LedCnt);
	}
	return NV_SUCCESS;
}


int nv_drv_trans_ptz_param(Nvc_Ptz_Param *pPtzParam, Nvc_Ptz_Info_S  *pPtzInfo)
{
    memset(pPtzParam, 0, sizeof(Nvc_Ptz_Param));
    if (pPtzInfo->u32PtzCapMask & NVC_PTZ_SUPP_HMOVE)
    {
        if ((pPtzInfo->u32HPerStepDegrees >> 31) == 0 && pPtzInfo->u32HPerStepDegrees > 0
            && (pPtzInfo->u32HorizontalMinSteps >> 31) == 0 && pPtzInfo->u32HorizontalMinSteps > 0)
        {
            pPtzParam->u32PtzCapMask |= NVC_PTZ_SUPP_HMOVE;
            pPtzParam->fHPerStepDegrees = pPtzInfo->u32HPerStepDegrees/10000.0;
            pPtzParam->u32HorizontalMinSteps = pPtzInfo->u32HorizontalMinSteps;
        }
    }
    if (pPtzInfo->u32PtzCapMask & NVC_PTZ_SUPP_VMOVE)
    {
        if ((pPtzInfo->u32VPerStepDegrees >> 31) == 0 && pPtzInfo->u32VPerStepDegrees > 0
            && (pPtzInfo->u32VerticalMinSteps >> 31) == 0 && pPtzInfo->u32VerticalMinSteps > 0)
        {
            pPtzParam->u32PtzCapMask |= NVC_PTZ_SUPP_VMOVE;
            pPtzParam->fVPerStepDegrees = pPtzInfo->u32VPerStepDegrees/10000.0;
            pPtzParam->u32VerticalMinSteps = pPtzInfo->u32VerticalMinSteps;
        }
    }
    if ((pPtzParam->u32PtzCapMask & NVC_PTZ_SUPP_HMOVE & NVC_PTZ_SUPP_VMOVE) && (pPtzInfo->u32PtzCapMask & NVC_PTZ_SUPP_HVMOVE))
    {
        pPtzParam->u32PtzCapMask |= NVC_PTZ_SUPP_HVMOVE;
    }
	NV_DEBUG("u32PtzCapMask:%d",pPtzInfo->u32PtzCapMask);

	NV_DEBUG("u32HPerStepDegrees:%d,u32HorizontalMinSteps:%d,u32VPerStepDegrees:%d,u32VerticalMinSteps:%d",
		pPtzInfo->u32HPerStepDegrees,
		pPtzInfo->u32HorizontalMinSteps,
		pPtzInfo->u32VPerStepDegrees,
		pPtzInfo->u32VerticalMinSteps);


	NV_DEBUG("fHPerStepDegrees:%f,u32HorizontalMinSteps:%d,fVPerStepDegrees:%f,u32VerticalMinSteps:%d",
		pPtzParam->fHPerStepDegrees,
		pPtzParam->u32HorizontalMinSteps,
		pPtzParam->fVPerStepDegrees,
		pPtzParam->u32VerticalMinSteps);
	
    return NV_SUCCESS;
}

int nv_drv_init_ptz_info(int nFd, Nvc_Ptz_Param *pPtzParam)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
    Nvc_Ptz_Info_S stPtzInfo;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_PTZ_INFO_REQ : NVC_OLD_QUERY_PTZ_INFO_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Ptz Info Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_PTZ_INFO_RESP : NVC_OLD_QUERY_PTZ_INFO_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Ptz Info Msg Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < sizeof(uint32))
    {
        NV_ERROR("Get Ptz Info Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    memset(&stPtzInfo, 0, sizeof(Nvc_Ptz_Info_S));
    nSize = pDrvMsg->stMsgHdr.u16MsgLen > sizeof(Nvc_Ptz_Info_S) ? sizeof(Nvc_Ptz_Info_S) : pDrvMsg->stMsgHdr.u16MsgLen;
    memcpy(&stPtzInfo, &pDrvMsg->stCapInfo, nSize);
    nv_drv_trans_ptz_param(pPtzParam, &stPtzInfo);
    return NV_SUCCESS;
}

int nv_drv_get_night_light_status(int nFd, nv_plugin_nightlight_t *pNl)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_NIGHT_LIGHT_STATUS_REQ : NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Night light Status Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_NIGHT_LIGHT_STATUS_RESP : NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Night light Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Night light Status Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	PLGIN_DEBUG("light status :[%d], u8Luminance :[%d]!", pDrvMsg->stLightStatus.u8Status, pDrvMsg->stLightStatus.u8LumLevel);
    pNl->enNightLightStatus = pDrvMsg->stLightStatus.u8Status == 0 ? NV_NIGHTLIGHT_STATUS_OFF : NV_NIGHTLIGHT_STATUS_ON;
    pNl->u8Luminance = pDrvMsg->stLightStatus.u8LumLevel;
    return NV_SUCCESS;
}

int nv_drv_get_cur_temperature(int nFd, NV_FLOAT *pTemperature)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_TEMPERATURE_VALUE_REQ : NVC_OLD_QUERY_TEMPERATURE_VALUE_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Temperature Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_TEMPERATURE_VALUE_RESP : NVC_OLD_QUERY_TEMPERATURE_VALUE_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Temperature Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Temperature Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	PLGIN_DEBUG("s32Temperature :[%d]!", pDrvMsg->stTemperature.u32Temperature);
    NV_S32 s32Temperature = pDrvMsg->stTemperature.u32Temperature;
    if (s32Temperature > -27315 && s32Temperature < 50000)
    {
        *pTemperature = s32Temperature/100.0;
    }
    return NV_SUCCESS;
}

int nv_drv_get_cur_humidity(int nFd, NV_FLOAT *pHumidity)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_HUMIDIT_VALUE_REQ : NVC_OLD_QUERY_HUMIDIT_VALUE_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Humidity Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_HUMIDIT_VALUE_RESP : NVC_OLD_QUERY_HUMIDIT_VALUE_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Humidity Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Temperature Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	PLGIN_DEBUG("u32Humidity :[%d]!", pDrvMsg->stHumidity.u32Humidity);
    if (pDrvMsg->stHumidity.u32Humidity >= 0 && pDrvMsg->stHumidity.u32Humidity <= 10000)
    {
        *pHumidity = pDrvMsg->stHumidity.u32Humidity/100.0;
    }
    return NV_SUCCESS;
}


int nv_drv_get_button_status(int nFd, int nButtonNo, NV_BUTTON_STATUS_E *pButtonStatus)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_BUTTON_STATUS_REQ : NVC_OLD_QUERY_BUTTON_STATUS_REQ, 0, nButtonNo);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Button Status Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_BUTTON_STATUS_RESP : NVC_OLD_QUERY_BUTTON_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Button Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Button Status Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }

	PLGIN_DEBUG("Get button %d status :[%d]!", nButtonNo, pDrvMsg->stButtonStatus.u8Status);
   *pButtonStatus = pDrvMsg->stButtonStatus.u8Status == Nvc_ButtonStatusUp ? NV_BUTTON_LOOSEN_UP : NV_BUTTON_PRESS_DOWN;
    return NV_SUCCESS;
}

int nv_drv_get_audio_plug_status(int nFd, NV_AUDIOPLUS_STATUS_E *pAudioPlug)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_AUDIO_PLUG_STATUS_REQ : NVC_OLD_QUERY_AUDIO_PLUG_STATUS_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Audio Plug Status Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_AUDIO_PLUG_STATUS_RESP : NVC_OLD_QUERY_AUDIO_PLUG_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Audio Plug Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Audio Plug Status Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	PLGIN_DEBUG("audio status :[%d]!", pDrvMsg->stAplugStatus.u8Status);
    *pAudioPlug = pDrvMsg->stAplugStatus.u8Status == 0 ? NV_AUDIOPLUS_STATUS_OFF : NV_AUDIOPLUS_STATUS_ON;
    return NV_SUCCESS;
}

int nv_drv_get_ldr_status(int nFd, NV_IRC_STATUS_E *pLdrStatus)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

    nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_LDR_STATUS_REQ : NVC_OLD_QUERY_LDR_STATUS_REQ, 0, 0);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Get Ldr Status Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_LDR_STATUS_RESP : NVC_OLD_QUERY_LDR_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Get Ldr Status Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
    {
        NV_ERROR("Get Ldr Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
	PLGIN_DEBUG("ldr status :[%d]!", pDrvMsg->stLdrStatus.u8Status);
    *pLdrStatus = pDrvMsg->stLdrStatus.u8Status == 0 ? NV_IRC_STATUS_NIGHT : NV_IRC_STATUS_DAY;	
    return NV_SUCCESS;
}

int nv_drv_set_filter_switch(int nFd, NV_S32 s32DayOrNight)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Ctrl Ir-filter status :[%d]!", s32DayOrNight);
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_IRC_SWITCH_REQ : NVC_OLD_CONTROL_IRC_SWITCH_REQ, sizeof(Nvc_Ircut_Control_S), 0);
    pDrvMsg->stIrcCtrl.u8Status = s32DayOrNight == 0 ? 0 : 1;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Ircut_Control_S);
    
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Ctrl Ir-filter Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_IRC_SWITCH_RESP : NVC_OLD_CONTROL_IRC_SWITCH_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Ctrl Ir-filter Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Ctrl Ir-filter Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    return NV_SUCCESS;
}

int nv_drv_set_IfrLight_switch(int nFd, NV_S32 s32DayOrNight)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Ctrl Lamp Switch :[%d]!", s32DayOrNight);
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_LAMP_SWITCH_REQ : NVC_OLD_CONTROL_LAMP_SWITCH_REQ, sizeof(Nvc_Lamp_Control_S), 0);
    pDrvMsg->stLampCtrl.u8Switch = s32DayOrNight == 0 ? 0 : 1;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Lamp_Control_S);
    
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Ctrl Lamp Switch Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_LAMP_SWITCH_RESP : NVC_OLD_CONTROL_LAMP_SWITCH_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Ctrl Lamp Switch Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Ctrl Lamp Switch Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    return NV_SUCCESS;
}

int nv_drv_get_IfrLight_switch(int nFd, NV_S32 *pS32Satus)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_LAMP_STATUS_REQ : NVC_OLD_QUERY_LAMP_STATUS_REQ, 0, 0);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Get IfrLight Status Request Error!");
		return NV_FAILURE;
	}
	if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_LAMP_STATUS_RESP : NVC_OLD_QUERY_LAMP_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
	{
		NV_ERROR("Wait Get IfrLight Status Resp Error!");
		return NV_FAILURE;
	}
	if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
	{
		NV_ERROR("Get IfrLight Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
		return NV_FAILURE;
	}
	PLGIN_DEBUG("IfrLight status :[%d]!", pDrvMsg->stLampStatus.u8Status);
	*pS32Satus = pDrvMsg->stLampStatus.u8Status; 
	return NV_SUCCESS;
}

int nv_drv_set_doorlock_switch(int nFd, NV_S32 s32OpenOrClose)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Ctrl DoorLock Switch :[%d]!", s32OpenOrClose);
    nv_drv_pack_msg_hdr(szBuffer, NVC_CONTROL_DOORLOCK_SET_REQ, sizeof(Nvc_DoorLock_Control_S), 0);
    pDrvMsg->stDoorLockCtrl.u8Switch = s32OpenOrClose == 0 ? 0 : 1;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_DoorLock_Control_S);
    
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Ctrl DoorLock Switch Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, NVC_CONTROL_DOORLOCK_SET_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Ctrl DoorLock Switch Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Ctrl DoorLock Switch Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    return NV_SUCCESS;
}

int nv_drv_get_doorlock_switch(int nFd, NV_S32 *pS32Satus)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	nSize = nv_drv_pack_msg_hdr(szBuffer, NVC_CONTROL_DOORLOCK_GET_REQ, 0, 0);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Get DoorLock Status Request Error!");
		return NV_FAILURE;
	}
	if ((nSize = nv_drv_wait_message(nFd, NVC_CONTROL_DOORLOCK_GET_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
	{
		NV_ERROR("Wait Get DoorLock Status Resp Error!");
		return NV_FAILURE;
	}
	if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
	{
		NV_ERROR("Get DoorLock Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
		return NV_FAILURE;
	}
	PLGIN_DEBUG("DoorLock status :[%d]!", pDrvMsg->stDoorLockCtrl.u8Switch);
	*pS32Satus = pDrvMsg->stDoorLockCtrl.u8Switch; 
	return NV_SUCCESS;
}

int nv_drv_set_doorbell_tap(int nFd, NV_S32 s32DoorBellTap)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Ctrl DoorBell Tap :[%d]!", s32DoorBellTap);
    nv_drv_pack_msg_hdr(szBuffer, NVC_CONTROL_DOORBELL_SET_REQ, sizeof(Nvc_DoorBell_Control_S), 0);
    pDrvMsg->stDoorBellCtrl.u8Switch = s32DoorBellTap == 0 ? 0 : 1;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_DoorBell_Control_S);
    
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Ctrl DoorBell Tap Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, NVC_CONTROL_DOORBELL_SET_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Ctrl DoorBell Tap Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Ctrl DoorBell Tap Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    return NV_SUCCESS;
}


int nv_drv_set_lens_switch(int nFd, NV_S32 s32DayOrNight)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Ctrl Lens Switch :[%d]!", s32DayOrNight);
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_LENS_SWITCH_REQ : NVC_OLD_CONTROL_LENS_SWITCH_REQ, sizeof(Nvc_Lens_Control_S), 0);
    pDrvMsg->stLensCtrl.u8SwitchLens = s32DayOrNight == 0 ? 0 : 1;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Lens_Control_S);
    
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Ctrl Lens Switch Request Error!");
        return NV_FAILURE;
    }
    if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_LENS_SWITCH_RESP : NVC_OLD_CONTROL_LENS_SWITCH_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
    {
        NV_ERROR("Wait Ctrl Lens Switch Resp Error!");
        return NV_FAILURE;
    }
    if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
    {
        NV_ERROR("Ctrl Lens Switch Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
        return NV_FAILURE;
    }
    return NV_SUCCESS;
}

int nv_drv_set_temperature_timer(int nFd, NV_S32 s32CapSecInv, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Set Temperature Timer :[%d]!", s32CapSecInv);
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_SET_TEMPERATURE_TIMER_REQ : NVC_OLD_SET_TEMPERATURE_TIMER_REQ, sizeof(Nvc_Temperature_Timer_S), 0);
    pDrvMsg->stTCapTimer.u32DistTime = s32CapSecInv;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Temperature_Timer_S);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Set Temperature Timer Request Error!");
        return NV_FAILURE;
    }
    if (nWaitResp)
    {
        if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_SET_TEMPERATURE_TIMER_RESP : NVC_OLD_SET_TEMPERATURE_TIMER_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
        {
            NV_ERROR("Wait Set Temperature Timer Resp Error!");
            return NV_FAILURE;
        }
        if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
        {
            NV_ERROR("Set Temperature Timer Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
            return NV_FAILURE;
        }
    }
    return NV_SUCCESS;
}

int nv_drv_set_humidity_timer(int nFd, NV_S32 s32CapSecInv, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	PLGIN_DEBUG("Set Humidity Timer :[%d]!", s32CapSecInv);
    nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_SET_HUMIDITY_TIMER_REQ : NVC_OLD_SET_HUMIDITY_TIMER_REQ, sizeof(Nvc_Humidity_Timer_S), 0);
    pDrvMsg->stHCapTimer.u32DistTime = s32CapSecInv;
    nSize = sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Humidity_Timer_S);
    if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
    {
        NV_ERROR("Transfer Set Humidity Timer Request Error!");
        return NV_FAILURE;
    }
    if (nWaitResp)
    {
        if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_SET_HUMIDIT_TIMER_RESP : NVC_OLD_SET_HUMIDIT_TIMER_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
        {
            NV_ERROR("Wait Set Humidity Timer Resp Error!");
            return NV_FAILURE;
        }
        if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
        {
            NV_ERROR("Set Humidity Timer Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
            return NV_FAILURE;
        }
    }
    return NV_SUCCESS;
}

int nv_drv_package_ptz_ctrl_msg(char *pBuffer, nv_plugin_ptz_t *pPtzCtrl, Nvc_Ptz_Param *pPtzParam)
{
	int nDirectionMask = 0;
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)pBuffer;
    nv_drv_pack_msg_hdr(pBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_PTZ_COMMON_REQ : NVC_OLD_CONTROL_PTZ_COMMON_REQ, sizeof(Nvc_Ptz_Control_S), 0);
	pDrvMsg->stPtzCtrl.u8Speed = 50;
	if (pPtzCtrl->u32DegreeHorizontal > 245*100 || pPtzCtrl->u32DegreeVertical > 100*100)
	{
		NV_ERROR("degree out of range");
		return NV_FAILURE;
	}
	/*
	NV_DEBUG("enDirectionVertical:%d,u32DegreeVertical:%d,enDirectionHorizontal:%d,u32DegreeHorizontal:%d",
		pPtzCtrl->enDirectionVertical,
		pPtzCtrl->u32DegreeVertical,
		pPtzCtrl->enDirectionHorizontal,
		pPtzCtrl->u32DegreeHorizontal);

	NV_DEBUG("fHPerStepDegrees:%f,fVPerStepDegrees:%f,u32HorizontalMinSteps:%d,u32VerticalMinSteps:%d",
		pPtzParam->fHPerStepDegrees,
		pPtzParam->fVPerStepDegrees,
		pPtzParam->u32HorizontalMinSteps,
		pPtzParam->u32VerticalMinSteps);
	*/
	if (pPtzCtrl->u32DegreeVertical > 0 && (pPtzCtrl->enDirectionVertical == NV_PTZ_DIRECTION_UP
		|| pPtzCtrl->enDirectionVertical == NV_PTZ_DIRECTION_DOWN) && (pPtzParam->u32PtzCapMask & NVC_PTZ_SUPP_VMOVE) != 0)
	{
		pDrvMsg->stPtzCtrl.u32VSteps = pPtzCtrl->u32DegreeVertical/(pPtzParam->fVPerStepDegrees*0.01);
		if (pDrvMsg->stPtzCtrl.u32VSteps < pPtzParam->u32VerticalMinSteps)
			pDrvMsg->stPtzCtrl.u32VSteps = pPtzParam->u32VerticalMinSteps;
		nDirectionMask |= (pPtzCtrl->enDirectionVertical == NV_PTZ_DIRECTION_UP ? 0x1 : 0x2);
	}

	if (pPtzCtrl->u32DegreeHorizontal > 0 && (pPtzCtrl->enDirectionHorizontal == NV_PTZ_DIRECTION_LEFT
		|| pPtzCtrl->enDirectionHorizontal == NV_PTZ_DIRECTION_RIGHT) && (pPtzParam->u32PtzCapMask & NVC_PTZ_SUPP_HMOVE) != 0)
	{
		pDrvMsg->stPtzCtrl.u32HSteps = pPtzCtrl->u32DegreeHorizontal/(pPtzParam->fHPerStepDegrees*0.01);
		if (pDrvMsg->stPtzCtrl.u32HSteps < pPtzParam->u32HorizontalMinSteps)
			pDrvMsg->stPtzCtrl.u32HSteps = pPtzParam->u32HorizontalMinSteps;
		nDirectionMask |= (pPtzCtrl->enDirectionHorizontal == NV_PTZ_DIRECTION_LEFT ? 0x4 : 0x8);
	}
	if (pDrvMsg->stPtzCtrl.u32HSteps == 0 && pDrvMsg->stPtzCtrl.u32VSteps == 0)
		return NV_FAILURE;
	
	switch(nDirectionMask)
	{
		case 1:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_UP;
			break;

		case 2:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_DOWN;
			break;
		
		case 4:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_LEFT;
			break;

		case 5:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_LEFT_UP;
			break;
		
		case 6:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_LEFT_DOWN;
			break;

		case 8:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_RIGHT;
			break;

		case 9:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_RIGHT_UP;
			break;
		
		case 10:
			pDrvMsg->stPtzCtrl.u8PtzCmd = NV_PTZ_RIGHT_DOWN;
			break;
		
		default:
			return NV_FAILURE;
	}
	return sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Ptz_Control_S);
}

int nv_drv_ctrl_ptz(int nFd, nv_plugin_ptz_t *pPtzCtrl, Nvc_Ptz_Param *pPtzParam, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	
	nSize = nv_drv_package_ptz_ctrl_msg(szBuffer, pPtzCtrl, pPtzParam);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Ptz Ctrl Request Request Error!");
		return NV_FAILURE;
	}
	
	if (nWaitResp)
	{
		if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_PTZ_COMMON_RESP : NVC_OLD_CONTROL_PTZ_COMMON_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
		{
			NV_ERROR("Wait Ptz Ctrl Request Resp Error!");
			return NV_FAILURE;
		}
		if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
		{
			NV_ERROR("Ptz Ctrl Request Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
			return NV_FAILURE;
		}
	}
	return NV_SUCCESS;
}

int nv_drv_package_night_light_ctrl_msg(char *pBuffer, nv_plugin_nightlight_t *pNlCtrl)
{
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)pBuffer;
    nv_drv_pack_msg_hdr(pBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_NIGHT_LIGHT_SWITCH_REQ : NVC_OLD_CONTROL_NIGHT_LIGHT_SWITCH_REQ, sizeof(Nvc_Night_Light_Control_S), 0);
	pDrvMsg->stLightCtrl.u8Switch = pNlCtrl->enNightLightStatus == NV_NIGHTLIGHT_STATUS_OFF ? 0 : 1;
	pDrvMsg->stLightCtrl.u8LumLevel = pNlCtrl->u8Luminance;
	return sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Night_Light_Control_S);
}

int nv_drv_ctrl_night_light(int nFd, nv_plugin_nightlight_t *pNlCtrl, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	
	nSize = nv_drv_package_night_light_ctrl_msg(szBuffer, pNlCtrl);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Night light Ctrl Request Error!");
		return NV_FAILURE;
	}
	if (nWaitResp)
	{
		if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_NIGHT_LIGHT_SWITCH_RESP : NVC_OLD_CONTROL_NIGHT_LIGHT_SWITCH_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
		{
			NV_ERROR("Night light Ctrl Resp Error!");
			return NV_FAILURE;
		}
		if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
		{
			NV_ERROR("Set light Ctrl Resp Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
			return NV_FAILURE;
		}
	}
	return NV_SUCCESS;
}

int nv_drv_get_night_light_switch(int nFd, NV_S32 *pS32Satus)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;

	nSize = nv_drv_pack_msg_hdr(szBuffer, g_fDriver_version >= 1.0 ? NVC_QUERY_NIGHT_LIGHT_STATUS_REQ : NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_REQ, 0, 0);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Get night light Status Request Error!");
		return NV_FAILURE;
	}
	if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_QUERY_NIGHT_LIGHT_STATUS_RESP : NVC_OLD_QUERY_NIGHT_LIGHT_STATUS_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
	{
		NV_ERROR("Wait Get night light Status Resp Error!");
		return NV_FAILURE;
	}
	if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS || pDrvMsg->stMsgHdr.u16MsgLen < 4)
	{
		NV_ERROR("Get night light Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
		return NV_FAILURE;
	}
	PLGIN_DEBUG("night light status :[%d]!", pDrvMsg->stLightStatus.u8Status);
	*pS32Satus = pDrvMsg->stLightStatus.u8Status; 
	return NV_SUCCESS;
}

int nv_drv_package_state_led_ctrl_msg(char *pBuffer, int nDevNo, nv_plugin_led_t *pLedCtrl)
{
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)pBuffer;
    nv_drv_pack_msg_hdr(pBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_STATE_LED_REQ : NVC_OLD_CONTROL_STATE_LED_REQ, sizeof(Nvc_State_Led_Control_S), nDevNo);
	pDrvMsg->stLedCtrl.eColor = NV_LED_COLOR_DEFAULT;//pLedCtrl->enLedColor;
	pDrvMsg->stLedCtrl.u32OnMesl = pLedCtrl->u32On*10;
	pDrvMsg->stLedCtrl.u32OffMesl = pLedCtrl->u32Off*10;
	return sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_State_Led_Control_S);
}

int nv_drv_ctrl_state_led(int nFd, int nDevno, nv_plugin_led_t *pLedCtrl, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	
	nSize = nv_drv_package_state_led_ctrl_msg(szBuffer, nDevno, pLedCtrl);
	printf("State Led: Device no = %d\n", nDevno);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer State Led Ctrl Request Error!");
		return NV_FAILURE;
	}
	if (nWaitResp)
	{
		if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_STATE_LED_RESP : NVC_OLD_CONTROL_STATE_LED_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
		{
			NV_ERROR("State Led Ctrl Resp Error!");
			return NV_FAILURE;
		}
		if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
		{
			NV_ERROR("Set State Led Resp Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
			return NV_FAILURE;
		}
	}
	return NV_SUCCESS;
}

int nv_drv_package_audio_plug_ctrl_msg(char *pBuffer, nv_plugin_audioplus_t *pAPlusCtrl)
{
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)pBuffer;
    nv_drv_pack_msg_hdr(pBuffer, g_fDriver_version >= 1.0 ? NVC_CONTROL_AUDIO_PLUG_SWITCH_REQ : NVC_OLD_CONTROL_AUDIO_PLUG_SWITCH_REQ, sizeof(Nvc_Audio_Plug_Control_S), 0);
	pDrvMsg->stAplugCtrl.u8Switch = pAPlusCtrl->enAudioPlusStatus == NV_AUDIOPLUS_STATUS_OFF ? 0 : 1;
	return sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_Audio_Plug_Control_S);
}
int nv_drv_ctrl_audio_plug(int nFd, nv_plugin_audioplus_t *pAPlusCtrl, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	
	nSize = nv_drv_package_audio_plug_ctrl_msg(szBuffer, pAPlusCtrl);
	if (nv_drv_send_message(nFd, szBuffer, nSize) != nSize)
	{
		NV_ERROR("Transfer Audio Plug Ctrl Request Error!");
		return NV_FAILURE;
	}
	if (nWaitResp)
	{
		if ((nSize = nv_drv_wait_message(nFd, g_fDriver_version >= 1.0 ? NVC_CONTROL_AUDIO_PLUG_SWITCH_RESP : NVC_OLD_CONTROL_AUDIO_PLUG_SWITCH_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
		{
			NV_ERROR("Audio Plug Ctrl Resp Error!");
			return NV_FAILURE;
		}
		if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
		{
			NV_ERROR("Set Audio Plug Resp Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
			return NV_FAILURE;
		}
	}
	return NV_SUCCESS;
}

int nv_drv_package_fm1288_msg(char *pBuffer, nv_plugin_fm1288_t *pFM1288)
{
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)pBuffer;
	int i = 0;
	//printf("pFM1288->u32DataLength = %d\n", pFM1288->u32DataLength);
	
    if (pFM1288->enFM1288CtrlType == NV_FM1288_CTRL_WRITE)
    {
    	//nv_drv_pack_msg_hdr(pBuffer, NVC_CONTROL_FM1288_WRITE_REQ, sizeof(Nvc_FM1288_Control_S), 0);
		nv_drv_pack_msg_hdr(pBuffer, NVC_CONTROL_FM1288_WRITE_REQ, pFM1288->u32DataLength, 0);
		printf("Package Head OK!\n");
		pDrvMsg->stFM1288.u8Data = pFM1288->u8Data;
    	pDrvMsg->stFM1288.u32DataLength = pFM1288->u32DataLength;
		
#if (1)
		printf("FM1288 Data Package Length = %d\n", pDrvMsg->stFM1288.u32DataLength);
		printf("Data Packaged: \n");		
		for (i = 0; i < pFM1288->u32DataLength; i++)
		{
			if (0xFC == pDrvMsg->stFM1288.u8Data[i])
			{
				printf("\n");
			}
			printf("0x%X  ", pDrvMsg->stFM1288.u8Data[i]);
		}
		printf("\tTotle: %d\n", i);
#endif
    }
    else
    {
    	nv_drv_pack_msg_hdr(pBuffer, NVC_CONTROL_FM1288_READ_REQ, sizeof(Nvc_FM1288_Control_S), 0);
    	pDrvMsg->stFM1288.u8RegHigh = pFM1288->u8RegHigh;
		pDrvMsg->stFM1288.u8RegLow = pFM1288->u8RegLow;
    }
	return sizeof(Nvc_Driver_Msg_Hdr_S) + sizeof(Nvc_FM1288_Control_S);
}

int nv_drv_ctrl_fm1288(int nFd, nv_plugin_fm1288_t *pFM1288, int nWaitResp)
{
	int nSize = 0;
	char szBuffer[256] = {0};
	int nWriteSize = 0;
	int i = 0;
    Nvc_Drv_Message *pDrvMsg = (Nvc_Drv_Message *)szBuffer;
	
	nSize = nv_drv_package_fm1288_msg(szBuffer, pFM1288);
#if DEBUG_FM1288
	printf("nv_drv_package_fm1288_msg nSize = %d\n", nSize);
#endif	
	if ((nWriteSize = nv_drv_send_message(nFd, szBuffer, nSize)) != nSize)
	{
		NV_ERROR("Transfer FM1288 Request Error!");
		
		return NV_FAILURE;
	}
	printf("Message Send OK!\n");
	if (pFM1288->enFM1288CtrlType == NV_FM1288_CTRL_WRITE)
	{
		if (nWaitResp)
		{
			if ((nSize = nv_drv_wait_message(nFd, NVC_CONTROL_FM1288_WRITE_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
			{
				NV_ERROR("FM1288 Write Resp Error!");
				return NV_FAILURE;
			}
		}
	}
	else
	{
		if (nWaitResp)
		{
			if ((nSize = nv_drv_wait_message(nFd, NVC_CONTROL_FM1288_READ_RESP, szBuffer, sizeof(szBuffer), 1000)) <= 0)
			{
				NV_ERROR("FM1288 Read Resp Error!");
				return NV_FAILURE;
			}
		}
	}
	
	if (pDrvMsg->stMsgHdr.u8ErrCode != NVC_DRIVER_SUCCESS)
	{
		NV_ERROR("Set State Led Resp Error %d, recv size :%d, msg size: %d!", pDrvMsg->stMsgHdr.u8ErrCode, nSize, pDrvMsg->stMsgHdr.u16MsgLen);
		return NV_FAILURE;
	}
	return NV_SUCCESS;
}
