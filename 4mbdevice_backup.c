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

#define MAJOR_NUMBER 66
#define DRIVER_AUTHOR "Rangarajan Kesavan A0163130X"
#define DRIVER_DESC "Assignemnt 4 - 4mb character device test"
#define BUF_LEN 1000

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

/* forward declaration */

int testdevice_open (struct inode *inode, struct file *filep);
int testdevice_release (struct inode *inode, struct file *filep);
ssize_t testdevice_read (struct file *filep, char __user * buf, size_t count, loff_t *f_pos);
ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos);
loff_t testdevice_llseek (struct file *filp, loff_t off, int whence);
static void testdevice_exit (void);


/* definition of file operation structure */

struct file_operations testdevice_fops = {
	read:	testdevice_read,
	write:	testdevice_write,
	llseek:	testdevice_llseek,
	open:	testdevice_open,
	release:testdevice_release
};

static char data[BUF_LEN];
//static char *data_ptr;
struct cdev *test_device;


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
	
	int maxbytes;
	int bytes_to_do;
	int bytes_read;

	maxbytes = BUF_LEN - *f_pos;

	if ( maxbytes > count ) bytes_to_do = count;
	else bytes_to_do = maxbytes;

	if ( bytes_to_do == 0 ){
		printk (KERN_INFO "Reached end of device \n");
		return -ENOSPC; /*EOF*/
	}
	
	bytes_read = bytes_to_do - copy_to_user(buf, data + *f_pos, bytes_to_do);
	*f_pos += bytes_read;

	printk (KERN_INFO "%d bytes are read from device\n", bytes_read);
	return bytes_read;
}

/*Write data to the device, effectively here its like writing to a file*/
ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos) {
	
	int bytes_written;
	int bytes_to_do;
	int maxbytes;

	maxbytes = BUF_LEN - *f_pos;

	if (maxbytes > count) bytes_to_do = count;
	else bytes_to_do = maxbytes;

	if ( bytes_to_do == 0){
		printk (KERN_INFO "Reached end of device \n");
		return -ENOSPC; /*EOF*/
	}

	bytes_written = bytes_to_do - copy_from_user( data + *f_pos, buf, bytes_to_do );
	*f_pos += bytes_written;

	printk (KERN_INFO "%d bytes written to the device\n", bytes_written);
	return bytes_written;
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
		case 2: /* END OF THE SEEK */
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

	/*
	//allocate 4MB for storage with kmalloc
	//to release memory use kfree
	
	data_ptr = kmalloc(4194304, GFP_KERNEL);
	if(!data_ptr) {
		testdevice_exit();
		//if any error in allocation of memory return negative
		return -ENOMEM;
	}
	*/
	//initialize data as 'X'
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
