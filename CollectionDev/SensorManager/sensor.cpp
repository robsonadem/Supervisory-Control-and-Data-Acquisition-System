#include "sensor.h"

Sensor::Sensor(std::string sname, std::string buff, int hubSN, int port, uint32_t sampPeriod){
    name = sname;
    buffer = buff;
    samplingPeriod = sampPeriod;
    PhidgetVoltageInput_create(&ch);
    Phidget_setIsHubPortDevice((PhidgetHandle) ch, 1);
    Phidget_setDeviceSerialNumber((PhidgetHandle) ch, hubSN);
    Phidget_setHubPort((PhidgetHandle) ch, port);
    // set Events Here
    PhidgetReturnCode res;

    res = Phidget_setOnAttachHandler(ch, onAttachHandler, this);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on attach handler\n");
        return (res);
    }

    res = Phidget_setOnDetachHandler(ch, onDetachHandler, this);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on detach handler\n");
        return (res);
    }
    // then Open
    Phidget_open((PhidgetHandle) *ch);
}

Sensor::~Sensor(){
    Phidget_close(ch);
    PhidgetVoltageInput_delete(&ch);
}

void CCONV
Sensor::onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
    Sensor *sen = static_cast<Sensor *>(ctx);
    printf("%s Voltage Changed: %.4f \n", *sen->name, voltage,);
    std::string vbuff = "";
    snprintf(vbuff, 100, "%d, %d, %f \n", *sen->name,voltage);
    *sen->buffer.append(vbuff);
}


void CCONV
Sensor::onAttachHandler(PhidgetHandle ch, void *ctx) {
    Sensor *sen = static_cast<Sensor *>(ctx);
    PhidgetVoltageInput_setDataInterval((PhidgetVoltageInputHandle) ch, *sen->samplingPeriod);
    PhidgetVoltageInput_setSensorValueChangeTrigger((PhidgetVoltageInputHandle) ch, 0);
}

void CCONV
Sensor::onDetachHandler(PhidgetHandle ch, void *ctx) {
    // log that it detached with ts
    std::cout << "bye" << std::endl;
}
