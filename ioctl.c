/* use this process to control the kernel module. Without using cat or echo we will
now be using the ioctl functions to read and write */

#include "testdevice.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Functions for the ioctl calls */

ioctl_set_msg (int file_desc, char *user_msg){

	int ret_val;
	ret_val = ioctl (file_desc, IOCTL_SET_MSG, user_msg);

	if (ret_val < 0){
		printf("ioctl_set_msg failed:%d\n", ret_val);
		exit(-1);
	}else{
		printf("New dev_msg set in device driver by IOW\n");
	}
}

ioctl_get_msg (int file_desc){
	
	int ret_val;
	char user_msg[1024];

	ret_val = ioctl(file_desc, IOCTL_GET_MSG, user_msg);

	if (ret_val < 0){
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
	}

	printf("Gettin msg from device with IOR:\n%s\n", user_msg);
}

ioctl_get_nth_byte(int file_desc, char *user_msg){
	int i;
	char c;

	printf("Executing _IOWR  original user_msg is :\n");

	i = 0;
	do {
		c = ioctl(file_desc, IOCTL_GET_NTH_BYTE, i++);

		if (c < 0){
			printf("ioctl_get_nth_byte failed at the %d'th byte:\n",i);
			exit(-1);
		}
		
		putchar(c);
	} while(c != 0);
	int ret_val;
	ret_val = ioctl (file_desc, IOCTL_SET_MSG, user_msg);
	putchar ('\n');
}

/* Main function to access the ioctl functions */

main() {
	printf ("Second message with IOWR\n");
	int file_desc, ret_val;
	char *user_msg = "Goodbye world,from ioctl user program\n";

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0){
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}

	ioctl_set_msg(file_desc, user_msg);	
	ioctl_get_nth_byte(file_desc,user_msg);	
	ioctl_get_msg(file_desc);
	
	close(file_desc);
}
