#ifndef V4L2SERIAL_H
#define V4L2SERIAL_H

#include <linux/videodev2.h>
#include <SerialStream.h>
//#include <linux/serial.h>

#define V4L_PIX_WIDTH   320
#define V4L_PIX_HEIGHT  240
#define V4L_PIX_FORMAT  V4L2_PIX_FMT_RGB332

extern void v4lOpenCamera(const char* file);

extern void v4lConnect2Serial(LibSerial::SerialStream *port);

//extern bool v4lOpenSerial(const char* file);

extern bool v4lReadCamera(char *buf);

extern void v4lSendImage2Serial();

extern void v4lRun(int priority);

#endif // V4L2SERIAL_H
