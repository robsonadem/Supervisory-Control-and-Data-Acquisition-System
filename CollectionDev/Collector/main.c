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

#include <sqlite3.h>

#define DB_PATH "../../scada.db"
#define SAMPLE_BACKUPFILE "sampleBackUp.txt"
#define DMSBUFFER_SMPLTHRESHOLD 60
#define DMSBUFFER_SMPL_TIME 5
#define MAXBUFFERED_SAMPLES 60
#define SAMPLE_PREFIX "insert into sampletable(sensorid,timestamp,rawdata,iscal,collectstate) values \0"
#define SAMPLE_POSTFIX "; \0"

struct dmsBuffer{
    int index;
    int locked;
    char* samples[MAXBUFFERED_SAMPLES];
    int record;
    char currentstate[50];
};

struct Sensor{
    int hub;
    int port;
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
        struct Sensor *sen;
        sen = (struct Sensor*) ctx;
        res = PhidgetVoltageInput_setDataInterval((PhidgetVoltageInputHandle) phid, sen->samplingPeriod);
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
        printf("SET DI %i %i %i\n",sen->hub, sen->port, check);

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
    if(errorCode == EEPHIDGET_SATURATION){
        fprintf(stderr, "SATURATION");
    }

}

static void CCONV
onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
    struct timespec tv;
    //gettimeofday(&tv, NULL);
    // use CLOCK_MONOTONIC for systems that want a time that will not be adjusted
    clock_gettime(CLOCK_REALTIME, &tv);
    unsigned long long millisecondsSinceEpoch =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_nsec)/ 1000000;

    int hubSN = -1;
    int hubPort = -1;
    Phidget_getDeviceSerialNumber((PhidgetHandle) ch, &hubSN);
    Phidget_getHubPort((PhidgetHandle) ch, &hubPort);

    // print to string buffer
    if(ctx){
        struct dmsBuffer* buffptr = (struct dmsBuffer*) ctx;
        if(buffptr->record){
            char msg[100] = ""; // 32 hardcode count for below (account for '\0')
            if(hubSN == 497194){
                snprintf(msg, (sizeof(msg)), "('s0',%llu,%f,0,%s)", millisecondsSinceEpoch, voltage, buffptr->currentstate);
            } else {
                snprintf(msg, (sizeof(msg)), "('s1',%llu,%f,0,%s)", millisecondsSinceEpoch, voltage, buffptr->currentstate);
            }
            while(buffptr->locked){

            }
            buffptr->locked = 1;
            buffptr->samples[buffptr->index] = msg;
            buffptr->index++;
            buffptr->locked = 0;
            printf("%s\n", msg);
            //printf("%s\n", buffptr->samples[buffptr->index - 1]);
        }
    }

    // file backup
    FILE *fp;
    fp = fopen(SAMPLE_BACKUPFILE, "a");
    fprintf(fp,"%d %d %llu %f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
    fclose(fp);

    // print to console/terminal
    //printf("%d %d %llu %f\n", hubSN, hubPort, millisecondsSinceEpoch, voltage);
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
    * Specifies that the channel should only match a VINT hub port.
    * The only valid channel id is 0.
    *
    * The default is 0 (false), meaning VINT hub ports will never match
    */
    Phidget_setIsHubPortDevice(ch, 1);

    return (EPHIDGET_OK);
}

int main(int argc, char **argv) {

    struct dmsBuffer buff;
    buff.index = 0;
    buff.locked = 0;
    buff.record = 0;
    strcat(buff.currentstate,"'IDLE'");

    // use readPipe to instantiate Map Sensor Architecture
    int numbSensors = 2;
    struct Sensor map[10];
    PhidgetReturnCode res;
    const char *errs;
    for(int i = 0; i < numbSensors; i++){
//        printf("%i\n", i);
//        // read HUB and Port
//        map[i].hub = 497194;
//        map[i].port = i;
//        map[i].samplingPeriod = 4000; // in msec

        if(i == 0){
            map[i].hub = 497194;
            map[i].port = 1;
            map[i].samplingPeriod = 2500;
        }
        if(i == 1){
            map[i].hub = 495331;
            map[i].port = 5;
            map[i].samplingPeriod = 750;
        }
        // make ch
        res = PhidgetVoltageInput_create(&map[i].ch);
        if (res != EPHIDGET_OK) {
            fprintf(stderr, "failed to create voltage input channel\n");
            exit(1);
        }
    }


    char *ermsg = 0;
    sqlite3 *db;
    sqlite3_open(DB_PATH, &db);
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &ermsg);
    sqlite3_exec(db, ".separator |", 0, 0, &ermsg);
    printf("after exec\n");
//    sqlite3_exec(db,
//                 "insert into "
//                 "sampletable(sensorid,timestamp,rawdata) "
//                 "values('s0',123212,2131231);",0,0, &ermsg);
    char test[] = "insert into sampletable(sensorid,timestamp,rawdata) values('s0',123212,2131231);";
    sqlite3_exec(db, test,0,0, &ermsg);
    printf("after exec 2\n");
    sqlite3_close(db);

    //printf("%s\n", ermsg);
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
        Phidget_setDeviceSerialNumber((PhidgetHandle) map[i].ch, map[i].hub);
        Phidget_setHubPort((PhidgetHandle) map[i].ch, map[i].port);
    }

    for(int i = 0; i < numbSensors; i++){
        res = PhidgetVoltageInput_setOnVoltageChangeHandler(map[i].ch, onVoltageChangeHandler, &buff);
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
    ssleep(3);


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

    // enter main loop
    struct timespec before, after, diff;
    clock_gettime(CLOCK_MONOTONIC, &before);
    printf("******** MAIN LOOP ********\n");
    buff.record = 1;
    while(1){        
        clock_gettime(CLOCK_MONOTONIC, &after);
        diff.tv_sec = after.tv_sec - before.tv_sec;
        diff.tv_nsec = after.tv_nsec - before.tv_nsec;
        if(((int) diff.tv_sec) > DMSBUFFER_SMPL_TIME || buff.index > MAXBUFFERED_SAMPLES - 5){
            before = after;            
            clock_gettime(CLOCK_MONOTONIC, &after);            
            printf("\n******** PUSH TO DMS ********\n");

            before = after;
            clock_gettime(CLOCK_MONOTONIC, &after);

            // multiple individual insert
            sqlite3_open(DB_PATH, &db);
            //sqlite3_exec(db, 0, 0, &ermsg);
            sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &ermsg);
            sqlite3_exec(db, ".separator |", 0, 0, &ermsg);
            for(int i = 0; i < buff.index; i++){
                char final[300] = "";
                printf("*** %s\n", buff.samples[i]);
                strcat(final, SAMPLE_PREFIX);
                strcat(final, buff.samples[i]);
                strcat(final, SAMPLE_POSTFIX);
                //printf("%s\n", final);
                sqlite3_exec(db, final, 0, 0, &ermsg);
                //printf("err: %s\n", ermsg);
            }
            sqlite3_close(db);
            // console print
            //printf("%s", &buff.samples);
            printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
            for(int i = 0; i < buff.index; i++){
                char clear[200] = " ";
                buff.samples[i] = clear;
            }
            buff.index = 0;
            FILE *fp;
            fp = fopen(SAMPLE_BACKUPFILE, "w");
            //fprintf(fp, "%s", &buff.samples);
            fclose(fp);
        }
    }


done:
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
