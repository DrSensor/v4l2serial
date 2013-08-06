#include "v4l2serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <pthread.h>

using namespace LibSerial;

SerialStream *serial;
int camera_fd;
bool serial_enable = false;
bool thread_start = false;
size_t size = V4L_PIX_WIDTH*V4L_PIX_HEIGHT*2;

pthread_mutex_t mutex;

static int xioctl(int fh, int request, void *arg)
{
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

void allocCamera(const char *file)
{
    struct v4l2_capability cap;
//    struct v4l2_crop crop;
    struct v4l2_format fmt;

    camera_fd = open(file, O_RDONLY);

    if (-1 == xioctl (camera_fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            perror (stderr, "%s is no V4L2 device\n",
                        file.c_str());
            exit (EXIT_FAILURE);
        } else {
            printf("\nError in ioctl VIDIOC_QUERYCAP\n\n");
            exit(0);
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        perror (stderr, "%s is no video capture device\n",
                file);
        exit (EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        perror (stderr, "%s does not support read i/o\n",
                file);
        exit (EXIT_FAILURE);
    }

    memset(&fmt, 0, sizeof(fmt));

    fmt.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = V4L_PIX_WIDTH;
    fmt.fmt.pix.height      = V4L_PIX_HEIGHT;
    size = fmt.fmt.pix.width*fmt.fmt.pix.height*2;

    fmt.fmt.pix.pixelformat = V4L_PIX_FORMAT;

    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    if (-1 == xioctl(camera_fd, VIDIOC_S_FMT, &fmt)) {
        perror("VIDIOC_S_FMT");

    }
}


void v4lConnect2Serial(SerialStream *port)
{
    serial = port;
    serial_enable = true;
}


int v4lReadCamera(char *buf)
{
    int stat;

    stat = read(camera_fd, buf, size);
    if (stat == -1)
        perror("read(2)");
    else if (stat != size)
        perror("size buf mismatch!");

    return stat;
}


void v4lSendImage2Serial()
{
    char *buf;
    int size;

    size = v4lReadCamera(buf);
    if (size > 0) serial->write(buf, size);
}


void *procV4L(void *arg)
{
    while (arg == NULL && thread_start) {
        if(serial_enable) v4lSendImage2Serial();
    }
}

void v4lRun(int priority)
{
    int error;
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t v4l_t;

    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);

    error = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if(error != 0)
        perror("error = %d\n",error);

    error = pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
    if(error != 0)
        perror("error = %d\n",error);

    memset(&param, 0, sizeof(param));
    param.sched_priority = priority;
    error = pthread_attr_setschedparam(&attr, &param);
    if(error != 0)
        perror("error = %d\n",error);

    thread_start = true;
    error = pthread_create(&v4l_t, &attr, procV4L, NULL);
    if(error != 0)
        perror("error = %d\n",error);
}
