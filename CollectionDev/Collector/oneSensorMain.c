////#include <stdio.h>
////#include <time.h>
////#include <string.h>

////int main()
////{
////    char* samples[1000] = "";
////    struct timeval tv;
////    for(int i = 0; i < 100; i++){
////        gettimeofday(&tv, NULL);

////        unsigned long long millisecondsSinceEpoch =
////                (unsigned long long)(tv.tv_sec) * 1000 +
////                (unsigned long long)(tv.tv_usec) / 1000;
////        char* vbuff[100];
////        snprintf(vbuff, 100, "%d, %d, %f", hubSN, hubPort, millisecondsSinceEpoch,voltage);
////        ("cat: %llu%llu\n", (unsigned long long) tv.tv_sec, ((unsigned long long) tv.tv_usec / 1000));
////        printf("act: %llu\n", millisecondsSinceEpoch);
////    }
////    printf("%llu\n", (unsigned long long)(tv.tv_sec));
////    printf("%llu\n", (unsigned long long)(tv.tv_sec) * 1000000);
////    printf("Hello World!\n");


////    return 0;
////}



//#include <stdio.h>
//#include <stdlib.h>

//#include <phidget22.h>

//#ifndef _WIN32
//#include <unistd.h>
//#else
//#include <Windows.h>
//#endif

//#include <time.h>
//#include <string.h>

//static void CCONV ssleep(int);

//static void CCONV
//onAttachHandler(PhidgetHandle phid, void *ctx) {
//    PhidgetReturnCode res;
//    int hubPort;
//    int channel;
//    int serial;

//    res = Phidget_getDeviceSerialNumber(phid, &serial);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to get device serial number\n");
//        return;
//    }

//    res = Phidget_getChannel(phid, &channel);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to get channel number\n");
//        return;
//    }

//    res = Phidget_getHubPort(phid, &hubPort);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to get hub port\n");
//        hubPort = -1;
//    }

//    if (hubPort == -1)
//        printf("channel %d on device %d attached\n", channel, serial);
//    else
//        printf("channel %d on device %d hub port %d attached\n", channel, serial, hubPort);
//}

//static void CCONV
//onDetachHandler(PhidgetHandle phid, void *ctx) {
//    PhidgetReturnCode res;
//    int hubPort;
//    int channel;
//    int serial;

//    res = Phidget_getDeviceSerialNumber(phid, &serial);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to get device serial number\n");
//        return;
//    }

//    res = Phidget_getChannel(phid, &channel);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to get channel number\n");
//        return;
//    }

//    res = Phidget_getHubPort(phid, &hubPort);
//    if (res != EPHIDGET_OK)
//        hubPort = -1;

//    if (hubPort != -1)
//        printf("channel %d on device %d detached\n", channel, serial);
//    else
//        printf("channel %d on device %d hub port %d detached\n", channel, hubPort, serial);
//}

//static void CCONV
//errorHandler(PhidgetHandle phid, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString) {

//    fprintf(stderr, "Error: %s (%d)\n", errorString, errorCode);
//}

//static void CCONV
//onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
//    int hubSN = -1;
//    int hubPort = -1;
//    Phidget_getDeviceSerialNumber((PhidgetHandle) ch, &hubSN);
//    Phidget_getHubPort((PhidgetHandle) ch, &hubPort);
//    struct timeval tv;
//    gettimeofday(&tv, NULL);
//    unsigned long long millisecondsSinceEpoch =
//            (unsigned long long)(tv.tv_sec) * 1000 +
//            (unsigned long long)(tv.tv_usec) / 1000;
//    printf("%d,%d,%llu,%.4f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
//    char vbuff[100];
//    snprintf(vbuff, 100, "%d,%d,%llu,%f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
//    strcat(ctx, vbuff);
//}

///*
//* Creates and initializes the channel.
//*/
//static PhidgetReturnCode CCONV
//initChannel(PhidgetHandle ch) {
//    PhidgetReturnCode res;

//    res = Phidget_setOnAttachHandler(ch, onAttachHandler, NULL);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to assign on attach handler\n");
//        return (res);
//    }

//    res = Phidget_setOnDetachHandler(ch, onDetachHandler, NULL);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to assign on detach handler\n");
//        return (res);
//    }

//    res = Phidget_setOnErrorHandler(ch, errorHandler, NULL);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to assign on error handler\n");
//        return (res);
//    }

//    /*
//    * Please review the Phidget22 channel matching documentation for details on the device
//    * and class architecture of Phidget22, and how channels are matched to device features.
//    */

//    /*
//    * Specifies the serial number of the device to attach to.
//    * For VINT devices, this is the hub serial number.
//    *
//    * The default is any device.
//    */
//    Phidget_setDeviceSerialNumber(ch, 497194);

//    /*
//    * For VINT devices, this specifies the port the VINT device must be plugged into.
//    *
//    * The default is any port.
//    */
//    Phidget_setHubPort(ch, 3);

//    /*
//    * Specifies that the channel should only match a VINT hub port.
//    * The only valid channel id is 0.
//    *
//    * The default is 0 (false), meaning VINT hub ports will never match
//    */
//    Phidget_setIsHubPortDevice(ch, 1);

//    return (EPHIDGET_OK);
//}

//int
//main(int argc, char **argv) {
//    PhidgetVoltageInputHandle ch;
//    PhidgetReturnCode res;
//    const char *errs;

//    char samples[1000];

//    /*
//    * Enable logging to stdout
//    */
//    //PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

//    res = PhidgetVoltageInput_create(&ch);
//    if (res != EPHIDGET_OK) {
//        fprintf(stderr, "failed to create voltage input channel\n");
//        exit(1);
//    }

//    res = initChannel((PhidgetHandle)ch);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to initialize channel:%s\n", errs);
//        exit(1);
//    }

//    res = PhidgetVoltageInput_setOnVoltageChangeHandler(ch, onVoltageChangeHandler, samples);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set voltage change handler: %s\n", errs);
//        goto done;
//    }

//    /*
//    * Open the channel synchronously: waiting a maximum of 5 seconds.
//    */
//    res = Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
//    if (res != EPHIDGET_OK) {
//        if (res == EPHIDGET_TIMEOUT) {
//            printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
//        } else {
//            Phidget_getErrorDescription(res, &errs);
//            fprintf(stderr, "failed to open channel:%s\n", errs);
//        }
//        goto done;
//    }

//    unsigned int* DI;
//    *DI = 500;
//    res = PhidgetVoltageInput_setDataInterval(ch, *DI);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set DataInterval: %s\n", errs);
//        goto done;
//    }

//    printf("Gathering data for 10 seconds...\n");
//    ssleep(10);

//done:
//    printf("%s", samples);
//    Phidget_close((PhidgetHandle)ch);
//    PhidgetVoltageInput_delete(&ch);

//    exit(res);
//}

//static void CCONV
//ssleep(int tm) {
//#ifdef _WIN32
//    Sleep(tm * 1000);
//#else
//    sleep(tm);
//#endif
//}
