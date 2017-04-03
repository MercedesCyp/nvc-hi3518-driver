#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>   
#include <string.h>   
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "NTC.h"
//#include <sys/sleep.h.h>
#define NV_SUCCESS  0
#define NV_FAILURE  (-1)

#define Channel_0 0U
#define Channel_1 1U


#define DC_TEMP_CHANNEL Channel_1
#define DC_INFRA_CHANNEL Channel_0

//990-600
#define SET_DUTY_CYCLE	0U
#define SET_VOLTAGE		1U



#define FAN_MOV_MIN_VOL	500
#define FAN_MOV_MAX_VOL	2000

#define DIV_FAN_TEMP_MIN  500
#define DIV_FAN_TEMP_ORD  650



//#define DC_SHIFT_NIGHT	0x0001
//#define DC_SHIFT_DAY	0x0100

#define DC_SHIFT_NIGHT	0x0100
#define DC_SHIFT_DAY	0x0200

//if the value is greater than PT_DAY. It is now in day
#define PT_DAY          (0x3a)
//if the value is smaller than PT_NIGHT. it is now in night 
#define PT_NIGHT        (0x2a)

#define DAY_NIGHT_BOUNDARY	((PT_DAY-PT_NIGHT)/2+PT_NIGHT)

#define SVR_BIND_PORT	59682

#define PIPE_NAME_SVR "/mnt/mtd/config/d11_irc_pipe_svr"
#define PIPE_NAME_CLI "/mnt/mtd/config/d11_irc_pipe_cli"
#if 0

int nv_create_local_udp_server(char *svrPath)
{
	int server_fd = 0;
	struct sockaddr_un server_address;
	int server_len= 0;

	if ((server_fd = socket(AF_UNIX, SOCK_DGRAM , 0)) < 0)
		return -1;

	memset(&server_address, 0, sizeof(struct sockaddr_un));

	server_address.sun_family = AF_UNIX;
	server_len = sizeof(server_address);

	if (sizeof(server_address.sun_path) < strlen(svrPath))
		strncpy(server_address.sun_path, svrPath, sizeof(server_address.sun_path)-1);
	else
		strncpy (server_address.sun_path, svrPath, strlen(svrPath));

	unlink (server_address.sun_path);

	if (bind(server_fd,(struct sockaddr*)&server_address, server_len) < 0)
	{
		close(server_fd);
		return -1;
	}
	
	return server_fd;
}

int nv_local_udp_send(int fd, char *svrPath, char *data, int nLen)
{
	struct sockaddr_un  server_address;

	memset(&server_address, 0, sizeof(server_address));
	
	server_address.sun_family = AF_UNIX;
	if (sizeof(server_address.sun_path) < strlen(svrPath))
		strncpy(server_address.sun_path, svrPath, sizeof(server_address.sun_path)-1);
	else
		strncpy (server_address.sun_path, svrPath, strlen(svrPath));

	if (nLen != sendto(fd, data, nLen, 0,(struct sockaddr *)&server_address, 
										 sizeof(struct sockaddr)))
	{
		return -1;
	}
	
	return nLen;
}

int nv_local_udp_recv(int fd, char *pData, int nLen)
{
	return recvfrom(fd, pData, nLen, 0, NULL, NULL);
}

int nv_local_udp_send_to_svr(char *SvrPath, char *data, int nLen)
{
	int s32Sock = 0;

	if (access(SvrPath, F_OK) == -1)
		return -1;

	if ((s32Sock = socket(AF_UNIX, SOCK_DGRAM , 0)) < 0)
		return -1;
	
	return nv_local_udp_send(s32Sock, SvrPath, data, nLen);
}
#endif


int nv_bind_sock(int sockfd, int nPort, int nAnyOrLocal)
{
	int nReuseAddr = 1;
    char *pIp = nAnyOrLocal == 0 ? "0.0.0.0" : "127.0.0.1";
    int ip = 0;
    struct sockaddr_in addr_local;
	memset(&addr_local, 0, sizeof(struct sockaddr_in));

    inet_pton(AF_INET, pIp, &ip);
	addr_local.sin_family      = AF_INET;
	addr_local.sin_addr.s_addr = ip;
	addr_local.sin_port        = htons(nPort);	
	
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&nReuseAddr,sizeof(int)) < 0)
		return NV_FAILURE;

	if (bind(sockfd,(struct sockaddr*)&addr_local, sizeof(struct sockaddr_in)) < 0)
		return NV_FAILURE;
    
	return NV_SUCCESS;
}

int nv_close_socket(int *pSock)
{
	if (pSock != NULL && *pSock > 0)
	{
		close(*pSock);
		*pSock = -1;
		return NV_SUCCESS;
	}
	return NV_FAILURE;	
}

int nv_create_udp_listen_sock()
{
    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        return -1;

    if (nv_bind_sock(sock, SVR_BIND_PORT, 1) < 0)
    {
        close(sock);
        return -1;
    }
	int flag = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flag | O_NONBLOCK);
    return sock;
}

#define DAY_SIGNAL		SIGUSR1
#define NIGHT_SIGNAL	SIGUSR2

int select_pipe_fd(int fd, int time_out)
{
	int maxfd = 0;
	int index = 0;
	int ret   = 0;
	
	fd_set read_fd, *p_read_fd;
	struct timeval timeO, *p_time_out;

	if (fd < 0)
		return -1;
	
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

int main(void)
{
	struct sockaddr_in addr;
	socklen_t fromlen = sizeof(addr);
	int HD_FAN,HD_TEMP,HD_LENS;
	unsigned int i=0;
	int pipe_exist_svr = 0, pipe_exist_cli = 0, pipe_fd_svr = -1, pipe_fd_cli = -1;
	int cam_pro_id = -1, last_status = -1, change_time = 0; // day 0, 1 night
	int ret;
	char szBuffer[1024];
	
	HD_TEMP = open("/dev/DIV_TEMPSENSOR",O_RDWR); 
	HD_FAN	= open("/dev/DIV_FAN",O_RDWR); 
	HD_LENS = open("/dev/SHITF_LENS",O_RDWR);

	if (HD_TEMP < 0 && HD_FAN < 0 && HD_LENS < 0)
	{
		return -1;
	}
	/*	
	unlink(PIPE_NAME_SVR);
	unlink(PIPE_NAME_CLI);
	if (access(PIPE_NAME_SVR,F_OK) == -1)
	{ 
		if (mkfifo(PIPE_NAME_SVR,0777) == 0)
			pipe_exist_svr = 1;
		else
			perror("create pipe error!"); 
	}

	if (access(PIPE_NAME_CLI,F_OK) == -1)
	{ 
		if (mkfifo(PIPE_NAME_CLI,0777) == 0)
			pipe_exist_cli = 1;
		else
			perror("create pipe error!"); 
	}
	if (pipe_exist_cli && pipe_exist_svr)
	{
		pipe_fd_svr = open(PIPE_NAME_SVR,O_RDWR | O_NONBLOCK);	 
		pipe_fd_cli = open(PIPE_NAME_CLI,O_RDWR | O_NONBLOCK);	 
	}
*/
	//pipe_fd_svr = nv_create_local_udp_server(PIPE_NAME_SVR);
	pipe_fd_svr = nv_create_udp_listen_sock();
	//for(i=0;i<30;i++)
	for(;;)
	{
		
		{
			int theTempVolVal,theTemperature;
			unsigned int theVoltage;
			ioctl(HD_TEMP,DC_TEMP_CHANNEL,&theTempVolVal);
			theTemperature = ConvertTempFromVoltage((uint16)theTempVolVal);
			if((theTemperature>DIV_FAN_TEMP_MIN)&&(theTemperature<DIV_FAN_TEMP_ORD)){
				theVoltage=FAN_MOV_MAX_VOL -(int)((theTemperature-DIV_FAN_TEMP_MIN)*10);
			}else if(theTemperature>DIV_FAN_TEMP_ORD){
				theVoltage = 100;
			}else{
				theVoltage = 3000;
			}
			//theVoltage=2500;
			//printf("**************************************************\r\n");
			//printf("TEMP		:%d \r\n",theTemperature);
			//printf("FUN VOLTAGE	:%d\r\n",theVoltage);
			ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
		}
		{
			int theLiVolVal;//Light sensor voltage
			ioctl(HD_TEMP,DC_INFRA_CHANNEL,&theLiVolVal); 
			//printf("LDR VOLTAGE	:%d\r\n",theLiVolVal);
			
			if( theLiVolVal > DAY_NIGHT_BOUNDARY )
			{
				if (last_status == -1 || last_status == 1)
				{
					ioctl(HD_LENS,DC_SHIFT_DAY,NULL);
					if (cam_pro_id > 0)
						kill(cam_pro_id, DAY_SIGNAL);
					last_status = 0;
					change_time = 0;
				}
				else
				{
					if (change_time++ < 5)
						ioctl(HD_LENS,DC_SHIFT_DAY,NULL);
				}
			}else{ 

				if (last_status == -1 || last_status == 0)
				{
					ioctl(HD_LENS,DC_SHIFT_NIGHT,NULL);
					if (cam_pro_id > 0)
						kill(cam_pro_id, NIGHT_SIGNAL);
					last_status = 1;
					change_time = 0;
				}
				else
				{
					if (change_time++ < 5)
						ioctl(HD_LENS,DC_SHIFT_NIGHT,NULL);
				}
			}
			//printf("**************************************************\r\n");		
		}
		/*
		
		
		//theVoltage=1200-100*i;
		printf("the voltage is :%d\r\n",theVoltage);
		ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
		*/
		ret = select_pipe_fd(pipe_fd_svr, 1000);
		if (ret > 0)
		{
			printf("Recv camera app message!!\n");
            //if (read(pipe_fd_svr, szBuffer, sizeof(szBuffer)) > 0)     
			//if (nv_local_udp_recv(pipe_fd_svr, szBuffer, sizeof(szBuffer)) > 0)    
			if (recvfrom(pipe_fd_svr, szBuffer, sizeof(szBuffer),  0, (struct sockaddr *)&addr, &fromlen) > 0)
        	{
				if (strcmp(szBuffer, "close") == 0)
				{
					cam_pro_id = -1;
				}
				else if (strstr(szBuffer, "camera_pid=") != NULL)
				{
					sscanf(szBuffer, "camera_pid=%d", &cam_pro_id);
					printf("camera app process id %d!!\n", cam_pro_id);
					if (cam_pro_id <= 0)
						cam_pro_id = -1;
					else
					{
						sprintf(szBuffer, "status=%d", last_status);
						//nv_local_udp_send_to_svr(PIPE_NAME_CLI, szBuffer,strlen(szBuffer));
						//if (pipe_fd_cli > 0)
						//write(pipe_fd_cli,szBuffer,strlen(szBuffer));
						sendto(pipe_fd_svr, szBuffer,strlen(szBuffer), 0,(struct sockaddr *)&addr, sizeof(addr));
					}
				}
        	}
			usleep(200*1000);
		}
		else if (ret < 0)
		{
			sleep(1);
		}
	}
	// theVoltage = 1300;
	// ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
	// sleep(6);
	// theVoltage = 1200;
	// ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
	// sleep(6);
	// theVoltage = 1100;
	// ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
	// sleep(6);
	// theVoltage = 1000;
	// ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
	// sleep(6);
	
	// theVoltage = 100;
	// ioctl(HD_FAN,SET_VOLTAGE,&theVoltage);
	// sleep(6);
	close(HD_FAN);
	close(HD_TEMP);
	close(HD_LENS);
	if (pipe_fd_svr > 0)
		close(pipe_fd_svr);
	if (pipe_fd_cli > 0)
		close(pipe_fd_cli);
	return 0;
}
