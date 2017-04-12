#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 66
#define DRIVER_AUTHOR "Rangarajan Kesavan A0163130X"
#define DRIVER_DESC "Assignemnt 4 - 4mb character device test"
#define BUF_LEN 1

static char Message[BUF_LEN];

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

/* forward declaration */

int testdevice_open (struct inode *inode, struct file *filep);
int testdevice_release (struct inode *inode, struct file *filep);
ssize_t testdevice_read (struct file *filep, char __user * buf, size_t count, loff_t *f_pos);
ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos);
static void testdevice_exit (void);


/* definition of file operation structure */

struct file_operations onebyte_fops = {
	read:	testdevice_read,
	write:	testdevice_write,
	open:	testdevice_open,
	release:testdevice_release
};

static char *testdevice_data;

int testdevice_open (struct inode *inode, struct file *filep) {	
	try_module_get(THIS_MODULE);
	return 0;
}

int testdevice_release (struct inode *inode, struct file *filep) {
	
	module_put(THIS_MODULE);
	return 0;
}

ssize_t testdevice_read (struct file *filep, char __user * buf, size_t count, loff_t *f_pos) {
	
	int bytes_read = 0;
	if (*testdevice_data == 0){
		return 0;
	}
	while ( count && *testdevice_data){
		put_user (*(testdevice_data++), buf++);
		count--;
		bytes_read++;
	}
	
	printk (KERN_INFO "%d bytes are read from device\n", bytes_read);
	return bytes_read;
}

ssize_t testdevice_write (struct file *filep, const char __user * buf, size_t count, loff_t *f_pos) {
	
	int i = 0;
	if ( count > 1 ){
		printk (KERN_INFO "More than one character given, only first character will be written to device\n");
	}

	for (i = 0; i < 1 && i < BUF_LEN; i++){
		get_user (Message[i], buf);
	}
	
	device_data = Message;

	printk (KERN_INFO "%d characters written to onebyte device\n", i);
	return i;
}

static int onebyte_init (void) {
	int result;
	//registering device
	result = register_chrdev(MAJOR_NUMBER, "testdevice", &testdevice_fops);
	if (result < 0) {
		return result;
	}

	//allocate one byte memory for storage with kmalloc
	//to release memory use kfree

	testdevice_data = kmalloc(sizeof(char), GFP_KERNEL);
	if(!testdevice_data) {
		testdevice_exit();
		//if any error in allocation of memory return negative
		return -ENOMEM;
	}

	//initialize data as 'X'
	printk (KERN_ALERT "This is test 4mb device module created by Rangarajan \n");
	return 0;
}

static void testdevice_exit(void) {
	//free the memory data and unregister the device
	if (testdevice_data) {
		kfree(testdevice_data);
		testdevice_data = NULL;
	}

	unregister_chrdev (MAJOR_NUMBER, "testdevice");
	printk (KERN_ALERT "Test 4mb device is now unloaded\n");
}

module_init (testdevice_init);
module_exit (testdevice_exit);
