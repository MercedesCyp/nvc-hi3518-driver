#include "GlobalParameter.h"
#include "PanTilt_FileOpts.h"
#include "PanTilt.h"

#include "DIV_ioctl.h"


#include <linux/types.h>
#include <linux/fs.h>// Regist the Drive num
#include <asm/uaccess.h>//access_ok get_put_user

static int __open (struct inode *_pinode, struct file *_pfile)
{
	printk("\r\nOPEN\r\n");
	//if(gfPanTiltIsBusy())
	//	return -EBUSY;
	return 0;
}

static int __close (struct inode *_pinode, struct file *_pfile)	
{
	printk("CLOSE\r\n");
	
	return 0;
}

static int __flush (struct file *_pinode, fl_owner_t _id)
{
	
	return 0;
}

static long __ioctl (struct file *_pfile, unsigned int _cmd, unsigned long _Uddr)
{
	int theRetVal;
	void __user* ptheUddr = (void __user*)_Uddr;
	int theUSet;
	printk("Input addr:%x\r\n",_Uddr);
	printk("Input cmd:%x\r\n",_cmd);
	printk("inter cmd:%x\r\n",IOCMD_PT_HClk_SS);
	
    if(_IOC_TYPE(_cmd) != IOCMD_NV_TYPE){
        return -ENOTTY;
    }

    if(( _IOC_NR(_cmd) < IOCMD_PT_Min )||( _IOC_NR(_cmd) > IOCMD_PT_Max ) ){
        return -ENOTTY;
    }
	
    if(_IOC_DIR(_cmd) & _IOC_READ){
        theRetVal = !access_ok(VERIFY_WRITE, (void *)_Uddr, _IOC_SIZE(_cmd)); 
		if(theRetVal) return -EFAULT;
		
    }else if(_IOC_DIR(_cmd) & _IOC_WRITE){
		theRetVal = copy_from_user(&theUSet, ptheUddr, sizeof(int));
		printk("CFU return:%d\r\n",theRetVal);
		printk("Input data:%d\r\n",theUSet);
		if(theRetVal) return -EFAULT;
	}
	
	if(gfPanTiltIsBusy())
		return -EBUSY;
	
	printk("it not busy!\r\n");
	
	switch(_cmd)
	{
		case IOCMD_PT_HClk_AS		:{
			theRetVal = gfStartSMMove(1,0,DC_SSMM_HSet);
		}break;
		case IOCMD_PT_HAclk_AS		:{
			theRetVal = gfStartSMMove(1,0,DC_SSMM_HSet|DC_SSMM_HAntiClockWise);
		}break;
		case IOCMD_PT_VUp_AS		:{
			theRetVal = gfStartSMMove(0,1,DC_SSMM_VSet);
		}break;
		case IOCMD_PT_VDn_AS		:{
			theRetVal = gfStartSMMove(0,1,DC_SSMM_VSet|DC_SSMM_VAntiClockWise);
		}break;
		
		case IOCMD_PT_HClk_Dgr30	:{
			theRetVal = gfStartSMMove(30,0,DC_SSMM_HSet|DC_SSMM_HDgr);
		}break;
		case IOCMD_PT_HAclk_Dgr30	:{
			theRetVal = gfStartSMMove(30,0,DC_SSMM_HSet|DC_SSMM_HAntiClockWise|DC_SSMM_HDgr);
		}break;
		case IOCMD_PT_VUp_Dgr15		:{
			theRetVal = gfStartSMMove(0,15,DC_SSMM_VSet|DC_SSMM_VDgr);
		}break;
		case IOCMD_PT_VDn_Dgr15		:{
			theRetVal = gfStartSMMove(0,15,DC_SSMM_VSet|DC_SSMM_VAntiClockWise|DC_SSMM_VDgr);
		}break;
		
		case IOCMD_PT_HClk_SS		:{
			printk("Input data:%d\r\n",theUSet);
			theRetVal = gfStartSMMove(theUSet,0,DC_SSMM_HSet);
		}break;
		case IOCMD_PT_HAclk_SS		:{
			theRetVal = gfStartSMMove(theUSet,0,DC_SSMM_HSet|DC_SSMM_HAntiClockWise);
		}break;
		case IOCMD_PT_VUp_SS		:{
			theRetVal = gfStartSMMove(0,theUSet,DC_SSMM_VSet);
		}break;
		case IOCMD_PT_VDn_SS		:{
			theRetVal = gfStartSMMove(0,theUSet,DC_SSMM_VSet|DC_SSMM_VAntiClockWise);
		}break;
		
		case IOCMD_PT_HClk_DgrS		:{
			theRetVal = gfStartSMMove(theUSet,0,DC_SSMM_HSet|DC_SSMM_HDgr);
		}break;
		case IOCMD_PT_HAclk_DgrS	:{
			theRetVal = gfStartSMMove(theUSet,0,DC_SSMM_HSet|DC_SSMM_HAntiClockWise|DC_SSMM_HDgr);
		}break;
		case IOCMD_PT_VUp_DgrS		:{
			theRetVal = gfStartSMMove(0,theUSet,DC_SSMM_HSet|DC_SSMM_VDgr);
		}break;
		case IOCMD_PT_VDn_DgrS		:{
			theRetVal = gfStartSMMove(0,theUSet,DC_SSMM_HSet|DC_SSMM_HAntiClockWise|DC_SSMM_VDgr);
		}break;
	}
	
//	if(theRetVal)
	
	return 0;
}

struct file_operations PanTilt_fops = {
	.owner				= THIS_MODULE,
	.open				= __open,
	.release			= __close,
	.flush				= __flush,
	.unlocked_ioctl		= __ioctl,
};


struct miscdevice PanTile_dev={
	  .minor = PanTilt_MINOR,
	  .name  = DC_PanTilt_NAME,
	  .fops  = &PanTilt_fops
};

