#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fs.h>// Regist the Drive num
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

//#include <linux/moduleparam.h>
//#include <linux/version.h>
#include "GlobalParameter.h"
#include "PWM.h"


#define PWM_NAME 	"DIV_FAN"
#define DRV_VERSION "13.11.28 - test"
#define PWM_MINOR 	21




static long __ioctl (struct file *_pfile, unsigned int _cmd, unsigned long _Uddr)
{
	void __user *argp = (void __user *)_Uddr;
	uint32 _InputNum;
	long _Rct=0;
	copy_from_user(&_InputNum,argp,sizeof(uint32));
	
	//printk("/************************************/\r\n");
	switch(_cmd)
	{
		case SET_DUTY_CYCLE:
		{
			_Rct=PWM_ChangeTheDutyCycle(_InputNum);
		}
		break;
		case SET_VOLTAGE:
		{
			
			_Rct=PWM_ChangeTheOutPutVoltage(_InputNum);
		}
		break;
		default:break;
	}
	
	return _Rct;
	
}

static int __open (struct inode *_pinode, struct file *_pfile)
{
	printk("OPEN FAN_PWM/r/n");
	return 0;
}
static int __close (struct inode *_pinode, struct file *_pfile)	
{
	printk("CLOSE FAN_PWM/r/n");
	return 0;
}


static struct file_operations Div_fops = {
	.owner				= THIS_MODULE,
	.open				= __open,
	.release			= __close,
	.unlocked_ioctl		= __ioctl,
};

static struct miscdevice misc_dev={
	  .minor = PWM_MINOR,
	  .name  = PWM_NAME,
	  .fops  = &Div_fops
};

static  int ___init(void)
{
	printk("Hello World!\r\n");
	InitPWM();
	misc_register(&misc_dev);
	return 0;
}



static void ___exit(void)
{
	printk("Beybey World!\r\n");
	UninitPWM();
	misc_deregister(&misc_dev);
}



module_init(___init);
module_exit(___exit);

//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NETVEW");
MODULE_DESCRIPTION("This Drive is used to output a PWM squre wave!");
//MODULE_VIRSION(DRV_VERSION);
//MODULE_DEVICE_TABLE();
//MODULE_ALIAS();


/*
struct miscdevice  {
        int minor;
        const char *name;
        const struct file_operations *fops;
        struct list_head list;
        struct device *parent;
        struct device *this_device;
        const char *nodename;
        mode_t mode;
};
*/

/*
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
};
*/