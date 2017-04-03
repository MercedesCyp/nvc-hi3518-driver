//#include <linux/kernel.h>
//#include <linux/init.h>
//#include <linux/module.h>
//#include <linux/sched.h>
//#include <linux/types.h>
//#include <linux/fs.h>// Regist the Drive num
//#include <linux/uaccess.h>
//#include <linux/delay.h>
//#include <linux/slab.h>  //kmalloc

//#include <linux/moduleparam.h>
//#include <linux/version.h>
#include "DIV_main.h"




mDIV_Regist gDIV_Regist={
	.aDrvName = DC_PanTilt_NAME,
	.afModeInital = gfInitSMHIVPort,
	.afModeUninital = gfUninitSMHIVPort,
	.aDriverInfo = &PanTile_dev
};


static  int ___init(void)
{
	printk("Start DIV!\r\n");
	
	gDIV_Regist.afModeInital();
	
	misc_register(gDIV_Regist.aDriverInfo);
	return 0;
}



static void ___exit(void)
{
	printk("Exit DIV!\r\n");
	
	gDIV_Regist.afModeUninital();
	
	misc_deregister(gDIV_Regist.aDriverInfo);
}



module_init(___init);
module_exit(___exit);

MODULE_AUTHOR("Build by maj. 2015-1-23");
MODULE_LICENSE("GPL");
//MODULE_DESCRIPTION();
//MODULE_LICENSE("Dual BSD/GPL");

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