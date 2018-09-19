#include <iostream>
#include <time.h>
#include <stdint.h>

#include <phidget22.h>

using namespace std;
// I think this needs to be a run function in sensormanager
// use threading to start it
int main(){
    // Setup
    // open samplePipe, confirm open message

    // Confirm samplePipe open message from Collector

    // Open config pipe to Collector

    // Send open confirm message over config pipe

    // check samplePipe for confirmation of open config pipe

    // create SensorManager object

    // read Sensor to HUB/Port config into sensor manager

    // get sensor currentStateConfig from db and push to the config pipe
    // maybe do this through sensor manager

    while(true){
        // check if bufferDMSTime has past
        if(true){
            // push to dms
        }
        // check if current state has changed in SensorManager
        if(true){
            // get new configs from dms
            // push through configs config
        }
        // check if there is stuff in samplePipe
        if(true){
            // getline
            // check if the line is a sample
            if(true){
                // look up sensorname by hub_sn and port
                // put it in sample string in sensor manager
            }
            // check if the line is a sensor status change (by attach/dettach event)
            if(true){

            }
        }
    }
    return 0;
}


