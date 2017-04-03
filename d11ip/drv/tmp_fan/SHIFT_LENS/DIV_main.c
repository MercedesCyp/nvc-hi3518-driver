#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fs.h>// Regist the Drive num
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/slab.h>  //kmalloc

//#include <linux/moduleparam.h>
//#include <linux/version.h>
#include "GlobalParameter.h"
#include "REGShiftLens.h"


mFilePrivdata *thePrivdata = NULL ;

static long __ioctl (struct file *_pfile, unsigned int _cmd, unsigned long _Uddr)
{
	long  theRetVal = 0;
	//printk("Im in\r\n");
	switch( _cmd )
	{
		case DC_SHIFT_NIGHT:
		{
			//printk("OPEN NIGHT\r\n");
			F_ShiftToNightMode();
		}break;
		case DC_SHIFT_DAY:
		{
			//printk("OPEN DAY\r\n");
			F_ShiftToDayMode();
		}break;
		default:
		{
			
		}break;
	}
	return theRetVal;
}

static int __open (struct inode *_pinode, struct file *_pfile)
{
	printk("\r\nOPEN SHITF_LENS\r\n");
	if( thePrivdata!= NULL )
	{
		thePrivdata->aOpenTimes++;
	}
	return 0;
}
static int __close (struct inode *_pinode, struct file *_pfile)	
{
	printk("CLOSE\r\n");
	return 0;
}
static int __flush (struct file *_pinode, fl_owner_t _id)
{
	if( thePrivdata!= NULL )
	{
		thePrivdata->aOpenTimes--;
	}
	return 0;
}

static struct file_operations Div_fops = {
	.owner				= THIS_MODULE,
	.open				= __open,
	.release			= __close,
	.flush				= __flush,
	.unlocked_ioctl		= __ioctl,
	
};

static struct miscdevice misc_dev={
	  .minor = SHIFT_LEN_MINOR,
	  .name  = SHIFTLEN_NAME,
	  .fops  = &Div_fops
};

static  int ___init(void)
{
	printk("Start Shift_lens_drive!\r\n");
	thePrivdata  =  kmalloc(sizeof( mFilePrivdata ), GFP_KERNEL);
	thePrivdata->aOpenTimes = 0;
	
	F_InitShiftLens();
	
	misc_register(&misc_dev);
	return 0;
}



static void ___exit(void)
{
	printk("Exit Shift_lens_drive!\r\n");
	kfree(thePrivdata);
	
	F_UninitShiftLens();
	
	misc_deregister(&misc_dev);
}



module_init(___init);
module_exit(___exit);

MODULE_AUTHOR("Build by maj. 2014-12-12");
MODULE_DESCRIPTION("This Drive is used to output a PWM squre wave!");
MODULE_LICENSE("GPL");
//MODULE_LICENSE("Dual BSD/GPL");

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