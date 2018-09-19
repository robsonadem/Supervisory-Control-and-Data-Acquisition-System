#include <iostream>

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    return 0;#include <stdio.h>

        #include <stdio.h>
        #include <stdlib.h>

        #include <phidget22.h>

        #ifndef _WIN32
        #include <unistd.h>
        #else
        #include <Windows.h>
        #endif

        int main(int argc, char **argv) {
            PhidgetVoltageInputHandle ch1;
            PhidgetReturnCode res1;
            PhidgetVoltageInputHandle ch2;
            PhidgetReturnCode res2;
            const char *errs;


            res1 = PhidgetVoltageInput_create(&ch1);
            res2 = PhidgetVoltageInput_create(&ch2);

            /* Specifies the serial number of the device to attach to.
            * For VINT devices, this is the hub serial number.
            *
            * The default is any device.
            */
            Phidget_setDeviceSerialNumber((PhidgetHandle)ch1, 497194);

            /*
            * For VINT devices, this specifies the port the VINT device must be plugged into.
            *
            * The default is any port.
            */
            Phidget_setHubPort((PhidgetHandle)ch1, 3);

            /*
            * Specifies that the channel should only match a VINT hub port.
            * The only valid channel id is 0.
            *
            * The default is 0 (false), meaning VINT hub ports will never match
            */
            Phidget_setIsHubPortDevice((PhidgetHandle)ch1, 1);
            Phidget_setIsHubPortDevice((PhidgetHandle)ch2, 1);

            /*
            * Specifies which channel to attach to.  It is important that the channel of
            * the device is the same class as the channel that is being opened.
            *
            * The default is any channel.
            */
            // Phidget_setChannel(ch, 0);

            /*
            * Open the channel synchronously: waiting a maximum of 5 seconds.
            */
            res1 = Phidget_openWaitForAttachment((PhidgetHandle)ch1, 5000);
            if (res1 != EPHIDGET_OK) {
                if (res1 == EPHIDGET_TIMEOUT) {
                    printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
                } exit(1);
            }

            res2 = Phidget_openWaitForAttachment((PhidgetHandle)ch2, 5000);
            if (res2 != EPHIDGET_OK) {
                if (res2 == EPHIDGET_TIMEOUT) {
                    printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
                } exit(1);
            }



            double val1 = 0.25;
            double val2 = 0.75;
            PhidgetVoltageInput_getMaxVoltage(ch1,&val1);
            PhidgetVoltageInput_getMaxVoltage(ch2,&val2);
            printf("Max             Max");
            printf("Value1: %.3f    Value2: %.3f \n", val1, val2);
            for(int i = 0; i < 100; i++){
                res1 = PhidgetVoltageInput_getVoltage(ch1,&val1);
                res2 = PhidgetVoltageInput_getVoltage(ch2,&val2);
                printf("Value1: %.3f    Value2: %.3f \n", val1, val2);
                if (res1 != EPHIDGET_OK) {
                    Phidget_getErrorDescription(res1, &errs);
                    fprintf(stderr, "read error: %s\n", errs);
                }
                if (res2 != EPHIDGET_OK) {
                    Phidget_getErrorDescription(res2, &errs);
                    fprintf(stderr, "read error: %s\n", errs);
                }
                Sleep(500);
            }
            Phidget_close((PhidgetHandle)ch1);
            PhidgetVoltageInput_delete(&ch1);
            Phidget_close((PhidgetHandle)ch2);
            PhidgetVoltageInput_delete(&ch2);
        }

}
