#ifndef SERIAL_H__
#define SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS (1)
#ifndef FAIL
#define FAIL (-1)
#endif
#define ERR_NOT_INITIALIZED (-2)
#define ERR_TIMEOUT (-3)

int Serial_Open(char const *Port);
void Serial_Close(void);
int Serial_Write(unsigned char *Buffer, unsigned long Size);
int Serial_Read(unsigned char *Buffer, unsigned long Size);

#ifdef __cplusplus
}
#endif
#endif
