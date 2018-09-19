#include "ControlSubSystem/modemanager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ControlSubSystem/branch.h"
#include "ControlSubSystem/state.h"
#include "../DMS/dms.h"
#include "../DMS/table.h"
#include "ControlSubSystem/calibration.h"
using namespace std;

int main()
{

    //ModeManager m("../ControlState_config.txt");
    // cout << "Mode Manager Configured" << endl;

    DMS db("../scada.db","../configuration_files/deftables_config.txt","../log.txt");

    db.loadConfigTable("control_config.txt","/Users/maxwellmcfarlane/scada_repo/configuration_files/control_config.txt");
    db.loadConfigTable("sensor_config.txt","/Users/maxwellmcfarlane/scada_repo/configuration_files/sensor_config.txt");

//    db.loadDataBase("../testbench_files/StateTableTB.txt");
//    db.loadDataBase("../testbench_files/BranchTableTB.txt");
//    db.loadDataBase("../testbench_files/ConditionTableTB.txt");


    //db.loadDataBase("../testbench_files/SensorTableTB.txt");
//    db.loadDataBase("../testbench_files/SensorConfTableTB.txt");


//    db.loadDataBase("../testbench_files/SampleTableTB.txt");
//    db.loadDataBase("../testbench_files/CalibrationSampleTableTB.txt");

    DMS *dbp=&db;


    try{
        ModeManager m("../configuration_files/mainConfig.txt", dbp);

        // Test for Mode Manager with data reaction
        m.configure();
        /*
        cout<<"CurrentState:"+m.currentState<<endl;
        m.nextstate();
        cout<<"CurrentState:"+m.currentState<<endl;
        m.nextstate();
        cout<<"CurrentState:"+m.currentState<<endl;
        m.nextstate();
        cout<<"CurrentState:"+m.currentState<<endl;*/
    }
    catch (const std::exception& e) {
        std::cout <<e.what();
    }

//    db.loadDataBase("../testbench_files/CalConfTableTB.txt");
//    Calibration c(dbp);
//    c.calibrate("1");


    // Unit test for state and state transition
    //                State s1("IDLE");
    //                State s2("RUN");
    //                State s3("FINISH");

    //                Branch b1(s2,"'s0':Select RawData from SampleTable where RawData  <0");
    //                Branch b2(s3,"'s0':Select RawData from SampleTable where RawData >60");

    //                s1.loadBranch(b1);
    //                s1.loadBranch(b2);
    //                //cout<<s1.numOfBranches;
    //                cout<<"Next state is: "+s1.nextstate(dbp)<<endl;


    return 0;
}
