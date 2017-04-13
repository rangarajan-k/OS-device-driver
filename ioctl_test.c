#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int lcd;

#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)

void test() {
	int k,i,sum;
	char s[3];
	
	memset(s, '2', sizeof(s));
	printf("Test begins! \n");

	k = write(lcd, s, sizeof(s));
	printf("written = %d\n",k);

	k = ioctl(lcd, SCULL_HELLO);
	printf("result = %d\n",k);
}

int main(int argc, char **argv){
	lcd = open("/dev/testdevice", O_RDWR);
	if (lcd == -1){
		perror("unable to open the testdevice");
		exit(EXIT_FAILURE);
	}

	test();
	close(lcd);
	return 0;
}
