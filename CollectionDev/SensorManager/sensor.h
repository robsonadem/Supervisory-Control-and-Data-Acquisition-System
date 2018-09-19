#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <phidget22.h>
#include <iostream>

#include <stdint.h>

class Sensor
{
public:
    std::string name; // name used by Database
    std::string buffer;
    uint samplingPeriod; // in ms

    PhidgetVoltageInputHandle *ch;

    Sensor(std::string sname,  std::string buff, int hubSN = 0, int port = 0, unsigned int sampPeriod = 5);

    ~Sensor();

//    // attaches the event for regular polling
//    void startPolling();

//    // unknown if this can happen
//    void stopPolling();

    void CCONV onVoltageChangeHandler(PhidgetVoltageInputHandle ch,
                                             void *ctx, double voltage);

    void CCONV
    onAttachHandler(PhidgetHandle ch, void *ctx);

    void CCONV
    onDetachHandler(PhidgetHandle ch, void *ctx);

}

#endif // SENSOR_H
