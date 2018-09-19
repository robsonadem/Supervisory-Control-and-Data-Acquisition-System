/* This is an archived copy of main that will preserve the usage of named
 * pipes. The other main will no longer use pipes as they are not necesary
 * if there is direct access to the db.
 */

#include <stdio.h>
#include <stdlib.h>

#include <phidget22.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

// includes for piping
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SAMPLEPIPE "/tmp/samplePipe"
#define SAMPLEPIPE_PORT 0666
#define SAMPLEPIPE_SETUP_MESSAGE "test"

#define SAMPLE_BACKUPFILE "sampleBackUp.txt"

// struct for map structure adapted from
// https://stackoverflow.com/questions/21958247/map-like-structure-in-c-use-int-and-struct-to-determine-a-value
// and the wikipedia page on structs
// utilizing linear search for Keys as there will not be an absurd ammount of sensors
// and this will not be called relatively often

struct Key{
    int hub;
    int port;
};

struct Sensor{
    struct Key key;
    PhidgetVoltageInputHandle ch;
    uint32_t samplingPeriod;
};

static void CCONV ssleep(int);

static void CCONV
onAttachHandler(PhidgetHandle phid, void *ctx) {
    PhidgetReturnCode res;
    int hubPort;
    int channel;
    int serial;

    if(ctx){
        struct Sensor *map;
        map = (struct Sensor*) ctx;
        res = PhidgetVoltageInput_setDataInterval((PhidgetVoltageInputHandle) phid, map->samplingPeriod);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to set device data interval\n");
            return;
        }
        uint32_t check = 0;
        res = PhidgetVoltageInput_getDataInterval((PhidgetVoltageInputHandle) phid, &check);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to set device data interval\n");
            return;
        }
        printf("SET DI %i %i %i\n",map->key.hub, map->key.port, check);

        res = PhidgetVoltageInput_setVoltageChangeTrigger((PhidgetVoltageInputHandle) phid, 0);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to set device voltage change trigger\n");
            return;
        }
    }

    res = Phidget_getDeviceSerialNumber(phid, &serial);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get device serial number\n");
        return;
    }

    res = Phidget_getChannel(phid, &channel);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get channel number\n");
        return;
    }

    res = Phidget_getHubPort(phid, &hubPort);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get hub port\n");
        hubPort = -1;
    }
    // send attach to SensorManger here
    if (hubPort == -1)
        printf("channel %d on device %d attached\n", channel, serial);
    else
        printf("channel %d on device %d hub port %d attached\n", channel, serial, hubPort);
}

static void CCONV
onDetachHandler(PhidgetHandle phid, void *ctx) {
    PhidgetReturnCode res;
    int hubPort;
    int channel;
    int serial;

    res = Phidget_getDeviceSerialNumber(phid, &serial);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get device serial number\n");
        return;
    }

    res = Phidget_getChannel(phid, &channel);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get channel number\n");
        return;
    }

    res = Phidget_getHubPort(phid, &hubPort);
    if (res != EPHIDGET_OK)
        hubPort = -1;

    if (hubPort != -1)
        printf("channel %d on device %d detached\n", channel, serial);
    else
        printf("channel %d on device %d hub port %d detached\n", channel, hubPort, serial);
}

static void CCONV
errorHandler(PhidgetHandle phid, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString) {

    fprintf(stderr, "Error: %s (%d)\n", errorString, errorCode);
}

static void CCONV
onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
    clock_t start_t, end_t, total_t;
    start_t = clock();

    struct timeval tv;
    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;

    int hubSN = -1;
    int hubPort = -1;
    Phidget_getDeviceSerialNumber((PhidgetHandle) ch, &hubSN);
    Phidget_getHubPort((PhidgetHandle) ch, &hubPort);

    // write to samplePipe
    int samplePipeFile;
    char* samplePipe = SAMPLEPIPE;
    samplePipeFile = open(samplePipe, O_WRONLY);
    printf("Writing to pipe\n");
    char msg[32]; // 32 hardcode count for below (account for '\0')
    snprintf(msg, (100*sizeof(char)), "%d %d %llu %f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
    strcat(msg, "\0");
    printf("%s", msg);
    int numBits = write(samplePipeFile, &msg, sizeof(msg));
    printf("Wrote to pipe\n");
    printf("%i\n", numBits);
    int n = close(samplePipeFile);
    printf("%i\n", n);
    printf("closed pipe file\n");

    FILE *fp;
    fp = fopen(SAMPLE_BACKUPFILE, "a");
    fprintf(fp,"%d %d %llu %f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
    fclose(fp);

    // print to console/terminal
    printf("%d %d %llu %f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);

    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time taken by CPU: %f\n", total_t);
}

/*
* Creates and initializes the channel.
*/
static PhidgetReturnCode CCONV
initChannel(PhidgetHandle ch, void *ctx) {
    PhidgetReturnCode res;

    res = Phidget_setOnAttachHandler(ch, onAttachHandler, ctx);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on attach handler\n");
        return (res);
    }

    res = Phidget_setOnDetachHandler(ch, onDetachHandler, NULL);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on detach handler\n");
        return (res);
    }

    res = Phidget_setOnErrorHandler(ch, errorHandler, NULL);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on error handler\n");
        return (res);
    }

    /*
    * Please review the Phidget22 channel matching documentation for details on the device
    * and class architecture of Phidget22, and how channels are matched to device features.
    */

    /*
    * Specifies the serial number of the device to attach to.
    * For VINT devices, this is the hub serial number.
    *
    * The default is any device.
    */
    //Phidget_setDeviceSerialNumber(ch, 497194);

    /*
    * For VINT devices, this specifies the port the VINT device must be plugged into.
    *
    * The default is any port.
    */
    //Phidget_setHubPort(ch, 3);

    /*
    * Specifies that the channel should only match a VINT hub port.
    * The only valid channel id is 0.
    *
    * The default is 0 (false), meaning VINT hub ports will never match
    */
    Phidget_setIsHubPortDevice(ch, 1);

    return (EPHIDGET_OK);
}

int main(int argc, char **argv) {

    // Create named pipe for writing samples
    mkfifo(SAMPLEPIPE, SAMPLEPIPE_PORT);

    // write to samplePipe for init
    int samplePipeFile;
    char* samplePipe = SAMPLEPIPE;
    samplePipeFile = open(samplePipe, O_WRONLY);
    //printf("Writing to pipe\n");
    char* msg = SAMPLEPIPE_SETUP_MESSAGE;
    int numBits = write(samplePipeFile, msg, sizeof(msg));
    //printf("Wrote to pipe\n");
    //printf("%i\n", numBits);
    int n = close(samplePipeFile);
    //printf("%i\n", n);
    //printf("closed pipe file\n");

    // use readPipe to instantiate Map Sensor Architecture
    int numbSensors = 4;
    struct Sensor map[numbSensors];
    PhidgetReturnCode res;
    const char *errs;
    for(int i = 0; i < numbSensors; i++){
        printf("%i\n", i);
        // read HUB and Port
        map[i].key.hub = 497194;
        map[i].key.port = i;
        map[i].samplingPeriod = 3000; // in msec
        // make ch
        res = PhidgetVoltageInput_create(&map[i].ch);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to create voltage input channel\n");
            exit(1);
        }
    }

    /*
    * Enable logging to stdout
    */
    //PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

    for(int i = 0; i < numbSensors; i++){
        res = PhidgetVoltageInput_create(&map[i].ch);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to create voltage input channel\n");
            exit(1);
        }
    }

    for(int i = 0; i < numbSensors; i++){
        res = initChannel((PhidgetHandle) map[i].ch, &map[i]);
        if (res != EPHIDGET_OK) {
            Phidget_getErrorDescription(res, &errs);
            fprintf(stderr, "failed to initialize channel:%s\n", errs);
            exit(1);
        }
    }

    for(int i = 0; i < numbSensors; i++){
        Phidget_setDeviceSerialNumber((PhidgetHandle) map[i].ch, map[i].key.hub);
        Phidget_setHubPort((PhidgetHandle) map[i].ch, map[i].key.port);
    }

    for(int i = 0; i < numbSensors; i++){
        res = PhidgetVoltageInput_setOnVoltageChangeHandler(map[i].ch, onVoltageChangeHandler, NULL);
        if (res != EPHIDGET_OK) {
            Phidget_getErrorDescription(res, &errs);
            fprintf(stderr, "failed to set voltage change handler: %s\n", errs);
            goto done;
        }
    }

    /*
    * Open the channel asynchronously: no waiting
    */
    for(int i = 0; i < numbSensors; i++){
        res = Phidget_open((PhidgetHandle) map[i].ch);
        if (res != EPHIDGET_OK) {
            if (res == EPHIDGET_TIMEOUT) {
                printf("Channel did not open\n");
            } else {
                Phidget_getErrorDescription(res, &errs);
                fprintf(stderr, "failed to open channel:%s\n", errs);
            }
            goto done;
        }
    }
    // give some time for sensors to attach
    ssleep(10);

    // check if channel is attached and report status
    int attached = 0;
    for(int i = 0; i < numbSensors; i++){
        res = Phidget_getAttached((PhidgetHandle) map[i].ch, &attached);
        if (res != EPHIDGET_OK) {
            if (res == EPHIDGET_TIMEOUT) {
                printf("Channel failed and error at getAttached\n");
            } else {
                Phidget_getErrorDescription(res, &errs);
                fprintf(stderr, "failed to open channel:%s\n", errs);
            }
            goto done;
        }
        if(attached){
            // report attached
        } else {
            // report detached
        }
    }

//    unsigned int* DI;
//    DI = malloc(sizeof(unsigned int));
//    *DI = 1000;
//    res = PhidgetVoltageInput_setDataInterval(ch1, *DI);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set DataInterval: %s\n", errs);
//        goto done;
//    }
//    *DI = 1000;
//    res = PhidgetVoltageInput_setDataInterval(ch2, *DI);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set DataInterval: %s\n", errs);
//        goto done;
//    }


    printf("Gathering data for 20 seconds...\n");
    ssleep(20);

    //printf("Gather data for 10 seconds with new dataInterval...\n");
//    *DI = 3000;
//    res = PhidgetVoltageInput_setDataInterval(ch1, *DI);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set DataInterval: %s\n", errs);
//        goto done;
//    }
//    *DI = 3000;
//    res = PhidgetVoltageInput_setDataInterval(ch2, *DI);
//    if (res != EPHIDGET_OK) {
//        Phidget_getErrorDescription(res, &errs);
//        fprintf(stderr, "failed to set DataInterval: %s\n", errs);
//        goto done;
//    }
//    free(DI);
//    ssleep(10);
done:
    //printf("%s", samples);

    unlink(SAMPLEPIPE);

    for(int i = 0; i < numbSensors; i++){
        Phidget_close((PhidgetHandle) map[i].ch);
        PhidgetVoltageInput_delete(&map[i].ch);
    }

    exit(res);
}

static void CCONV
ssleep(int tm) {
#ifdef _WIN32
    Sleep(tm * 1000);
#else
    sleep(tm);
#endif
}
