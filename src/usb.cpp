/*
 * usb.cpp
 *
 * This module has the wrapper functions to access USB driver functions.
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
*/

#ifdef Q_OS_WIN32
#include <setupapi.h>
#endif
#include <iostream>
#include "usb.h"
#include "hidapi.h"
/***************************************************
*                  GLOBAL VARIABLES
****************************************************/
static hid_device *DeviceHandle;	//Handle to write

static bool USBConnected = false;      //Boolean true when device is connected

static int defaultTimeOut = 20000;

unsigned int lastUSBTranstime;

int readTimeOut; //reset timeout in milliseconds

bool g_StartupCompleted;

bool USB_IsConnected()
{
    return USBConnected;
}

int USB_Init(void)
{
    readTimeOut = defaultTimeOut;
    USB_setLastTranstime();
    return hid_init();
}

int USB_Exit(void)
{
    return hid_exit();
}

int USB_Open()
{
    // Open the device using the VID, PID,
    // and optionally the Serial number.

    DeviceHandle = hid_open(MY_VID, MY_PID, NULL);

    if(DeviceHandle == NULL)
    {
        std::cout << "USB open failed." << std::endl;
        USBConnected = false;
        return -1;
    }
    USBConnected = true;
    return 0;
}

int USB_Write(void *pBuffer)
{
    if(DeviceHandle == NULL)
        return -1;

    USB_setLastTranstime();
    return hid_write(DeviceHandle, (const unsigned char *)pBuffer, USB_MIN_PACKET_SIZE+1);

}

int USB_Read(void *pBuffer)
{
    if(DeviceHandle == NULL)
        return -1;

    USB_setLastTranstime();
    return hid_read_timeout(DeviceHandle, (unsigned char *)pBuffer, USB_MIN_PACKET_SIZE, readTimeOut);
}

int USB_Close()
{
    hid_close(DeviceHandle);
    USBConnected = false;
    g_StartupCompleted = false;

    return 0;
}

void USB_setLastTranstime()
{

//#ifndef NO_TIMER_SUPPORT
//    QDateTime current = QDateTime::currentDateTime();
//    lastUSBTranstime = current.toTime_t();
//#endif
}

unsigned int USB_getLastTranstime()
{
    return lastUSBTranstime;
}
void USB_setDefaultTimeOut()
{

    readTimeOut = defaultTimeOut;
}

void USB_setTimeout(int time_ms)
{

    readTimeOut = time_ms;
}

