#include <iostream>
#include "v4l2serial.h"

using namespace std;
using namespace LibSerial;

SerialStream *serial;

bool openSerial(const char *file);

int main()
{
    serial = new SerialStream();
    openSerial("/dev/ttyUSB0"); // using usb2serial device in usb
//    openSerial("/dev/ttyS0"); // using serialpin

    v4lOpenCamera("/dev/video0");

    v4lConnect2Serial(serial);
//    v4lRun(90); // call this to run it in another thread/process

    while (1) {
        v4lSendImage2Serial();
    }

    return 0;
}

bool openSerial(const char *file)
{
    serial->Open(file);
    if(!serial->good()) {
        cerr << "Error: Could not open serial device !"
           << endl;
        return EXIT_FAILURE;
    }

    serial->SetBaudRate(SerialStreamBuf::BAUD_57600);
    if ( ! serial->good() ) {
        cerr << "Error: Could not set baudrate" <<
                endl ;
        return EXIT_FAILURE ;
    }

    serial->SetCharSize(SerialStreamBuf::CHAR_SIZE_DEFAULT);
    if ( ! serial->good() ) {
        cerr << "Error: Could not set the character size." <<
                endl ;
        return EXIT_FAILURE ;
    }

    serial->SetParity(SerialStreamBuf::PARITY_DEFAULT);
    if ( ! serial->good() ) {
        cerr << "Error: Could not set the parity" <<
                endl ;
        return EXIT_FAILURE ;
    }

    serial->SetNumOfStopBits(1);
    if ( ! serial->good() ) {
        cerr << "Error: Could not set the stop bit" <<
                endl ;
        return EXIT_FAILURE ;
    }

    serial->SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    if ( ! serial->good() ) {
        cerr << "Error: Could not set the flow control." <<
                endl;
        return EXIT_FAILURE ;
    }
}
