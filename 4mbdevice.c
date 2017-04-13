#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>

//#include "testdevice.h"
#define MAJOR_NUMBER 66
#define DRIVER_AUTHOR "Rangarajan Kesavan A0163130X"
#define DRIVER_DESC "Assignment 4 - 4mb character device test"
#define BUF_LEN 1024
//#define BUF_LEN 4194304
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

static char msg[BUF_LEN];
static char *msg_ptr;

/* forward declaration */

int testdevice_open (struct inode *inode, struct file *filep);
int testdevice_release (struct inode *inode, struct file *filep);
ssize_t testdevice_read (struct file *filep, char __user * buf, size_t count, loff_t *f_pos);
ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos);
loff_t testdevice_llseek (struct file *filp, loff_t off, int whence);
//int testdevice_ioctl (struct inode *inode, struct file *filep, unsigned int ioctl_num, unsigned long ioctl_param);
long testdevice_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static void testdevice_exit (void);

/* definition of file operation structure */

struct file_operations testdevice_fops = {
	read:	testdevice_read,
	write:	testdevice_write,
	llseek:	testdevice_llseek,
	unlocked_ioctl:	testdevice_ioctl,
	open:	testdevice_open,
	release:testdevice_release
};

int testdevice_open (struct inode *inode, struct file *filep) {	
	try_module_get(THIS_MODULE);
	return 0;
}

int testdevice_release (struct inode *inode, struct file *filep) {
	module_put(THIS_MODULE);
	return 0;
}

/*Read from the device and print the data in the user terminal*/
ssize_t testdevice_read (struct file *filep, char __user * buf, size_t count, loff_t *f_pos) {

	int bytes_read = 0;
	
	//if (*msg_ptr == 0)
	//	return 0;
	
	while (count && *msg_ptr){
		put_user (*(msg_ptr++), buf++);
		count--;
		bytes_read++;
	}

	printk (KERN_INFO "Read %d bytes from the testdevice\n", bytes_read);
	return bytes_read;

}

/*Write data to the device, effectively here its like writing to a file*/
ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos) {
	
	int i;

	for (i=0; i < count && i < BUF_LEN; i++)
		get_user(msg[i], buf+i);
	
	msg_ptr = msg;

	printk (KERN_INFO "%d bytes written to the testdevice\n", i);
	return i;

}

/*
int testdevice_ioctl (struct inode *inode, struct file *filep, unsigned int ioctl_num, unsigned long ioctl_param) {
	int i;
	char *temp;
	char ch;

	// Switch according to the IOCTL called
	switch (ioctl_num) {
	case IOCTL_SET_MSG:
		//Recieve a pointer to a message and set that to device's msg , get 
		//get the parameter given to ioctl by the proess

		temp = (char *)ioctl_param;
		get_user(ch, temp);
		for (i=0; ch && i < BUF_LEN; i++, temp++)
			get_user(ch, temp);

		testdevice_write(file, (char *)ioctl_param, i, 0);
		break;
	
	case IOCTL_GET_MSG:
		// Give the current message to the calling process - fill 
		//pointer we got
		i = testdevice_read(file, (char *)ioctl_param, 99, 0);
		
		//Put zero at end to properly terminate buffer
		put_user('\0', (char *)ioctl_param + i);
		break;
	
	case IOCTL_GET_NTH_BYTE:
		//Both input and output
		return msg[ioctl_param];
		brea;
	}

	return SUCCESS;
}
*/

long testdevice_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int err = 0;
	int retval = 0;

	//extract the type and number bitfields and don't decode wrong commands
	//return ENOTTY

	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user*)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch(cmd) {
		case SCULL_HELLO:
			printk(KERN_WARNING "hello\n");
			break;
		default:
			return -ENOTTY;
	}

	return retval;

}

loff_t testdevice_llseek (struct file *filp, loff_t offset, int whence) {
	
	loff_t newpos = 0;

	switch(whence) {
		case 0: /* SET THE SEEK */
			newpos = offset;
			break;
		case 1: /* CURSOR SEEK */
			newpos = filp->f_pos + offset;
			break;
		case 2: /* CURSOR END */
			newpos = BUF_LEN - offset;
			break;
		default: /* DONT DO ANYTHING */
			return -EINVAL;
	}
	
	if ( newpos > BUF_LEN ) newpos = BUF_LEN;
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

static int testdevice_init (void) {
	int result;
	//registering device
	result = register_chrdev(MAJOR_NUMBER, "testdevice", &testdevice_fops);
	if (result < 0) {
		return result;
	}
	
	msg_ptr = kmalloc(1024, GFP_KERNEL);
	
	printk (KERN_ALERT "This is test 4mb device module created by Rangarajan \n");
	return 0;
}

static void testdevice_exit(void) {
	//free the memory data and unregister the device
	//if (data_ptr) {
	//	kfree(data_ptr);
	//}

	unregister_chrdev (MAJOR_NUMBER, "testdevice");
	printk (KERN_ALERT "Test 4mb device is now unloaded\n");
}

module_init (testdevice_init);
module_exit (testdevice_exit);
