// testdevice.h - the header file with the ioctl definitions

#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include <linux/ioctl.h>

#define MAJOR_NUM 66

/* Set message of the device driver */
#define IOCTL_SET_MSG _IOW(MAJOR_NUM, 0, char *)

/* Get the message of the device driver */
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)

/* Get the n'th byte of the message from the device driver */
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUM, 2, int)

/* Name of the device file */
#define DEVICE_FILE_NAME "/dev/testdevice"

#endif
