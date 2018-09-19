#ifndef MODEMANAGER_H
#define MODEMANAGER_H
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "branch.h"
#include "state.h"
#include "../DMS/dms.h"
#include "../DMS/table.h"
#include "../tools/log.h"
/**
    Purpose: A Mode Manager Class that manages a number of state and transition with conditions verified by the DMS

    @author Robson Adem
    @version 1.0 12/03/17
*/

class State;
class DMS;

class ModeManager
{

public:
    string fileName;
    vector<State> states;
    vector<string> sensors;
    string currentState;
    DMS* db;
    /**
        * @brief     Given a configuration fileName and a DMS pointer it creates a mode manager
        * @param     string fileName - a fileName where a configuration script is written
        * @param     DMS * db - a pointer to the DMS
        */
    ModeManager(string fileName, DMS* db);
    /**
        * @brief     Given a state name it returns a pointer to the state and if the name does not exist it returns a NULL state
        * @param     string name -  the name of the state
        * @return    State* - a pointer to a state if found or a pointer to a NULL state
        */
    State *getState(string name);
    /**
        * @brief     checks if a sensor exists
        * @param     string name -  the name of the sensor
        * @return    bool true/false
        */
     bool  sensorExists(string name);
    /**
        * @brief     Configures the mode mangaer using the scripts written in the configuration file.
        */
    void configure();
    /**
        * @brief     It branches to the nextstate if the branch condiiton is found to be true.
        */
    void nextstate();
    /**
        * @brief     Distructor
        */
    ~ModeManager();
    /**
        * @brief     Given a string and a delimter , it splits strings.
        * @param     string s- a string to be split
        * @param     char  delimiter - a character to split the string with.
        * @return    vector<string>- split tokens
        */
    vector<string> split(const string s, char delimiter);

    /**
        * @brief     Given a condition it prepares a command query for the DMS.
        * @param     string condition - a condition to query
        * @return    string -  a refined query for the DMS
        */
    string prepQuery(string condition);


    Log * errorLog = new Log("../error_files/control_error.txt");//error log files.
    Log * log = new Log("../log_files/control_log.txt");// processs log files


};

#endif // MODEMANAGER_H
