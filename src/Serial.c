/**
 *
 * This module provides C callable APIs for communication with the PCSerial port.
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/types.h>
#include <fcntl.h>
//#include <errno.h>
#include <termios.h>

//#include "Error.h"
#include "Serial.h"
#include "Common.h"

static int UART_Device = -1;

/************************ FUNCTION DEFINITIONS*******************************/
/*
 * return SUCCESS, FAIL
 */
int Serial_Open(char const *DevName)
{
    struct termios Setting;

    UART_Device = open(DevName, O_RDWR | O_NOCTTY | O_NDELAY);

    if(UART_Device < 0)
//		THROW(FAIL);
        return FAIL;

    /*tcgetattr(UART_Device, &oldtio); */ /* save current port settings */

    bzero(&Setting, sizeof(Setting));

    Setting.c_cflag = B115200 | CS8 | CLOCAL | CREAD;

    Setting.c_iflag = 0;
    Setting.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    Setting.c_lflag = 0;
    Setting.c_cc[VTIME] = (uint8)(10000 / 100);   /* inter-character timer unused */
    Setting.c_cc[VMIN] = 0;   					/* blocking read until 1 chars received */

    tcflush(UART_Device, TCIFLUSH);

    tcsetattr(UART_Device, TCSANOW, &Setting);

    cfsetospeed(&Setting, B115200);
    cfsetispeed(&Setting, B115200);

    return SUCCESS;
}


int Serial_Write(unsigned char *Data, unsigned long Length)
{
    if(UART_Device < 0) {
//		THROW(ERR_NOT_INITIALIZED);
        printf("ERROR: Serial not initialized.\n");
        return ERR_NOT_INITIALIZED;
    }

    ssize_t retVal = write(UART_Device, Data, Length);
    if(retVal < 0) {
//		THROW(FAIL);
        printf("ERROR: Serial write failed.\n");
        return FAIL;
    }

    //printf("INFO: Serial write success.\n");
//    return SUCCESS;
    return (int)retVal;
}

int Serial_WaitForData(uint32 Timeout)
{
    fd_set InputFD;
    struct timeval TimeoutTimer;
    struct timeval *Timer;

    FD_ZERO(&InputFD);
    FD_SET(UART_Device, &InputFD);

    if(Timeout)
    {
        TimeoutTimer.tv_usec = Timeout;
        TimeoutTimer.tv_sec = 0;
        Timer = &TimeoutTimer;
    }
    else
    {
        Timer = NULL;
    }

    if(select(UART_Device + 1, &InputFD, NULL, NULL, Timer) == 0)
        return ERR_TIMEOUT;

    return SUCCESS;
}

int Serial_TimeoutReadData(unsigned char *Data, uint32 Length, uint32 InitTimeout, uint32 Timeout)
{
    int Bytes;

    if(UART_Device < 0) {
//		THROW(ERR_NOT_INITIALIZED);
        printf("ERROR: Serial_TimeoutReadData(): not init.\n");
        return ERR_NOT_INITIALIZED;
    }

    while(1)
    {
        if(ioctl(UART_Device, FIONREAD, &Bytes) < 0) {
//			THROW(FAIL);
            printf("ERROR: Serial_TimeoutReadData(): ioctl failed.\n");
            return FAIL;
        }

        if(Bytes)
        {
            if(Bytes > Length)
                Bytes = Length;

            if(read(UART_Device, Data, Bytes) < 0) {
//				THROW(FAIL);
                printf("ERROR: Serial_TimeoutReadData(): read failed.\n");
                return FAIL;
            }

            Length -= Bytes;
            Data = (uint8*)Data + Bytes;

            if(Length == 0)
                break;

            InitTimeout = Timeout;
        }

        if(Serial_WaitForData(InitTimeout) == ERR_TIMEOUT) {
//			THROW_S(ERR_TIMEOUT);
            printf("ERROR: read timeout.\n");
            return ERR_TIMEOUT;
        }

    }

    return Length > 0 ? Length : SUCCESS;
}

int Serial_Read(unsigned char *Data, unsigned long Length)
{
    return Serial_TimeoutReadData(Data, Length, 0, 0);
}


int UART_ClearRxFIFO(void)
{
    return tcflush(UART_Device, TCIFLUSH) < 0 ? FAIL : SUCCESS;
}

int UART_ClearTxFIFO(void)
{
    return tcflush(UART_Device, TCOFLUSH) < 0 ? FAIL : SUCCESS;
}

void Serial_Close(void)
{
    if(UART_Device > 0)
    {
        close(UART_Device);
        UART_Device = -1;
    }
}
