#include "modemanager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


ModeManager::ModeManager(string fileName, DMS *db)
{
    //store
    this->fileName=fileName;
    this->db=db;
}

ModeManager::~ModeManager()
{

}

State* ModeManager::getState(string name){
    for(int i=0;i< states.size();i++){
        if(!states[i].name.compare(name)){
            return &states[i];// the name matches return the pointer to this state
        }
    }
    State defaultState("NULL");//the name is not found in the states list return a NULL name state
    return &defaultState;
}
bool ModeManager::sensorExists(string name){
    for(int i=0;i< sensors.size();i++){
        if(!sensors[i].compare(name)){
            return true; // the sensor name matches return true
        }
    }
    return false;//// the sensor name does not match return fasle
}
void ModeManager::configure(){
    ifstream file;
    int lineCount=0;
    file.open(fileName.c_str());
    errorLog->trunc_file();//configure is called clear any preexisiting error log files
    log->trunc_file();//configure is called clear any preexisiting  log files


    if (!file){
        //file not found
        *log<<"CFG: File " + fileName + " couldn't be found!\n";
        *errorLog<<"CFG: File " + fileName + " couldn't be found!\n";
        throw runtime_error("CFG: File " + fileName + " couldn't be found!\n");
    }

    //Look for "STATES:" in a line, when found declare STATES
    string stateDeclaration("STATES:");
    //Look for "BRANCHES:" in a line, when found declare BRANCHES
    string branchDeclaration("BRANCHES:");
    //Look for "SENSORS" in a line, when found put configuration details into the dms
    string sensorDeclaration("SENSORS:");
    //Look for "SENSORS" in a line, when found put configuration details into the dms
    string calDeclaration("CALIBRATION:");
    //Look for "SENSORS" in a line, when found put configuration details into the dms
    string collDeclaration("COLLECTION:");
    //Look for "END" in a line, when found cease reading scripts
    string endDeclaration("END");


    string line;

    while (!file.eof())
    {
        //iterate till the end of the file

        getline(file,line);//iterator by line
        lineCount++;//line counter
        if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
        if(line.at(0) == '#')  continue; // # is for commenting;


        else if (line.find(stateDeclaration) != std::string::npos) {
            //state declaration block
            getline(file,line);
            lineCount++;//line counter
            vector<string > stateLine= ModeManager::split(line,',');//split lines
            for(string s: stateLine){

                State tempState(s);//create a state
                *log<<"CFG: State " +tempState.name + " has been successfully created!\n";
                states.push_back(tempState);//add the state to the states list
                db->getTable("StateTable")->addToTable("'"+s+"'");//add the state to the statetable in the dms
            }
        }


        if (line.find(branchDeclaration) != std::string::npos)
        {
            //branch declaration block
            while(!file.eof())
            {

                //iterate till end of the file
                getline(file,line);
                lineCount++;//line counter
                if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
                if(line.find(sensorDeclaration)!= std::string::npos) break;//this triggers the beginning of sensor declaration
                else if(line.find(calDeclaration)!= std::string::npos) break;//this triggers the beginning of calibration declaration
                if(line.at(0) == '#') continue;// # is for commenting
                else{
                    vector<string > branchLine= ModeManager::split(line,',');
                    if(branchLine.size()>3){
                        //more than 3 parameter errors
                        *errorLog<<"CFG_ERROR: In line number "<<lineCount<<" you gave "+to_string(branchLine.size()) +" parameters. The branch declaration is expecting 3 parameters: <STATE_FROM>,<STATE_TO>,CONDITION>!\n";
                        throw runtime_error("CFG_ERROR: parameter list more than 3!The branch declaration is expecting 3 parameters: <STATE_FROM>,<STATE_TO>,CONDITION>!");
                    }
                    else{
                        string from=ModeManager::getState(branchLine[0])->name;
                        //the first string in the line represents the state from where the branch starts
                        //the getStateMethod returns an actual state name decclared in the configuration or NULL if state has not been declared

                        if(!from.compare("NULL")){
                            //undeclared state error
                            *log<<"CFG_ERROR: State " + branchLine[0] + " is not in your state declaration!\n";
                            *errorLog<<"CFG_ERROR: State " + branchLine[0] + " is not in your state declaration!\n";
                            throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+" State " + branchLine[0] + " is not in your state declaration!\n");
                        }
                        else{
                            string to= ModeManager::getState(branchLine[1])->name;
                            if(!to.compare("NULL")){
                                //undeclared state error

                                *log<<"CFG_ERROR: In line "+to_string(lineCount)+" "+branchLine[1] + " is not in your state declaration!\n";
                                *errorLog<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+branchLine[1] + " is not in your state declaration!\n";
                                throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+ " "+branchLine[1] + " is not in your state declaration!\n");

                            }
                            else{
                                Branch tempBranch( getState( branchLine[1]),prepQuery(branchLine[2]));//create a branch
                                db->getTable("BranchTable")->addToTable("'"+from+"' , '"+to+"' , '"+branchLine[2]+"'");//add the branch to the DMS

                                for(int i=0;i< states.size();i++){
                                    if(!branchLine[0].compare(states[i].name)) states[i].loadBranch(tempBranch);//add the branch to state_from
                                }
                                *log<<"CFG: A Branch from State " + from +" to State " + tempBranch.branchState->name +" with the condition "+tempBranch.condition+" has been successfully loaded!\n";
                            }

                        }
                    }

                }

            }
        }

        if(line.find(sensorDeclaration)!= std::string::npos){

            while(!file.eof())
            {

                //iterate till end of the file
                getline(file,line);
                lineCount++;//line counter
                if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
                if(line.at(0) == '#') continue;// # is for commenting
                if(line.find(calDeclaration)!= std::string::npos) break;//this triggers the beginning of calibration declaration
                else{
                    vector<string > sensorLine= ModeManager::split(line,',');
                    if(sensorLine.size()>3){
                        //more than 3 parameter errors
                        *errorLog<<"CFG_ERROR: In line number "<<lineCount<<" you gave "+to_string(sensorLine.size()) +" parameters. The Sensor declaration is expecting 3 parameters: <HUB_ID>,<PORT>,<SENSOR_NAME>!\n";
                        throw runtime_error("CFG_ERROR: In line number "+to_string(lineCount)+ " you gave "+to_string(sensorLine.size()) +" parameters. The Sensor declaration is expecting 3 parameters: <HUB_ID>,<PORT>,<SENSOR_NAME>!\n");
                    }
                    else{

                        sensors.push_back(sensorLine[2]);//add the name of the sensor

                        *log<<"CFG: Sensor configured with "<<sensorLine[0]<<","<<sensorLine[1]<<","<<sensorLine[2]<<"\n";

                        string cmd = "'";
                        cmd += sensorLine[2];
                        cmd += "'," ;
                        cmd += sensorLine[0];
                        cmd += ",";
                        cmd += sensorLine[1];
                        cmd += ", null";
                        *log<<"CFG: Sensor configured with "<<cmd<<"\n";
                        db->getTable("SensorTable")->addToTable(cmd); //add the branch to the DMS
                    }

                }
            }

        }
        if(line.find(calDeclaration)!= std::string::npos) {
            while(!file.eof())
            {

                //iterate till end of the file
                getline(file,line);
                lineCount++;//line counter
                if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
                if(line.find(collDeclaration)!= std::string::npos) break;//this triggers a beginning
                if(line.at(0) == '#') continue;// # is for commenting
                else{
                    vector<string > calibrationLine= ModeManager::split(line,',');
                    if(calibrationLine.size()>3){
                        //more than 3 parameter errors
                        *errorLog<<"CFG_ERROR: In line number "<<lineCount<<" you gave "+to_string(calibrationLine.size()) +" parameters. The Calibration declaration is expecting 3 parameters: <SENSOR_NAME>,<STATE_NAME>,<Type:Coefficents(seprated by comma)>!\n";
                        throw runtime_error("CFG_ERROR: In line number "+to_string(lineCount)+ " you gave "+to_string(calibrationLine.size()) +" parameters. The Calibration declaration is expecting 3 parameters: <SENSOR_NAME>,<STATE_NAME>,<Type:Coefficents(separated by :)>!\n");
                    }
                    else{

                        if(!sensorExists(calibrationLine[0])){
                            //undeclared sensor name error
                            *log<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n";
                            *errorLog<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n";
                            throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n");

                        }
                        string state=ModeManager::getState(calibrationLine[1])->name;

                        //the second string in the line represents the state for the calibration is assigned to
                        //the getStateMethod returns an actual state name decclared in the configuration or NULL if state has not been declared

                        if(!state.compare("NULL")){
                            //undeclared state error

                            *log<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n";
                            *errorLog<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n";
                            throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n");
                        }

                        *log<<"CFG: Calibration Configuration with stateId:" + calibrationLine[0]+" stateName:"+calibrationLine[1]+" calibration:"+calibrationLine[2]+" has been successfully added to the DMS!\n";
                        string cmd = "'";
                        cmd += calibrationLine[0];
                        cmd += "','" ;
                        cmd += calibrationLine[1];
                        cmd += "','";
                        cmd += calibrationLine[2];
                        cmd += "'";
                        db->getTable("CalConfTable")->addToTable(cmd); //add the branch to the DMS


                        *log<<"CFG: Calibration Configuration with stateId:" + calibrationLine[0]+" stateName:"+calibrationLine[1]+" calibration:"+calibrationLine[2]+" has been successfully added to the DMS!\n";
                        db->getTable("CalConfTable")->addToTable("'"+calibrationLine[0]+"','"+calibrationLine[1]+"','"+calibrationLine[2]+"'");//add the calibration line to DMS
                    }


                }


            }
        }

        if(line.find(collDeclaration)!= std::string::npos) {
            while(!file.eof())
            {

                //iterate till end of the file
                getline(file,line);
                lineCount++;//line counter
                if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
                if(line.at(0) == '#') continue;// # is for commenting
                else{
                    vector<string > calibrationLine= ModeManager::split(line,',');
                    if(calibrationLine.size()>3){
                        //more than 3 parameter errors
                        *errorLog<<"CFG_ERROR: In line number "<<lineCount<<" you gave "+to_string(calibrationLine.size()) +" parameters. The Calibration declaration is expecting 3 parameters: <SENSOR_NAME>,<STATE_NAME>,<Type:Coefficents(seprated by comma)>!\n";
                        throw runtime_error("CFG_ERROR: In line number "+to_string(lineCount)+ " you gave "+to_string(calibrationLine.size()) +" parameters. The Calibration declaration is expecting 3 parameters: <SENSOR_NAME>,<STATE_NAME>,<Type:Coefficents(separated by :)>!\n");
                    }
                    else{

                        if(!sensorExists(calibrationLine[0])){
                            //undeclared sensor name error
                            *log<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n";
                            *errorLog<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n";
                            throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[0] + " is not in your sensor declaration!\n");

                        }
                        string state=ModeManager::getState(calibrationLine[1])->name;

                        //the second string in the line represents the state for the calibration is assigned to
                        //the getStateMethod returns an actual state name decclared in the configuration or NULL if state has not been declared

                        if(!state.compare("NULL")){
                            //undeclared state error

                            *log<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n";
                            *errorLog<<"CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n";
                            throw runtime_error("CFG_ERROR: In line "+to_string(lineCount)+ " "+calibrationLine[1] + " is not in your state declaration!\n");
                        }

                        *log<<"CFG: Calibration Configuration with stateId:" + calibrationLine[0]+" stateName:"+calibrationLine[1]+" calibration:"+calibrationLine[2]+" has been successfully added to the DMS!\n";
                        string cmd = "'";
                        cmd += calibrationLine[0];
                        cmd += "','" ;
                        cmd += calibrationLine[1];
                        cmd += "','";
                        cmd += calibrationLine[2];
                        cmd += "'";
                        db->getTable("SensorConfTable")->addToTable(cmd); //add the branch to the DMS

                          //cout<<cmd<<endl;
                        *log<<"CFG: Calibration Configuration with stateId:" + calibrationLine[0]+" stateName:"+calibrationLine[1]+" calibration:"+calibrationLine[2]+" has been successfully added to the DMS!\n";
                        db->getTable("CalConfTable")->addToTable("'"+calibrationLine[0]+"','"+calibrationLine[1]+"','"+calibrationLine[2]+"'");//add the calibration line to DMS
                    }


                }


            }
        }
        if(line.find(endDeclaration)!= std::string::npos) break;// this triggers an end to the script
    }
    file.close();
    if(!states.empty()) {
        currentState=(states[0]).name;
        db->setCurrentState(currentState);//update the dms
        //set the first state to be the default currentstate
        *log<<"CFG: DONE! State " + currentState + " is assigned to be the default current state.\n";
    }
}



void ModeManager::nextstate(){
    for(int i=0;i< states.size();i++){

        if(!states[i].name.compare(currentState)){
            //check if the next state branch can be taken
            *log<<"Now currrentState is: "+states[i].nextstate(db)+"\n";
            currentState=states[i].nextstate(db);//set whateever you got from the nextstate call to the current state
            db->setCurrentState(currentState);//update the currentstate in the dms
            //break;
        }
    }

}

vector<string> ModeManager::split(const string s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

string ModeManager::prepQuery(string condition){
    vector<string> tokens=split(condition,':');
    string str("'");
    str.append(tokens[0]);
    str.append("'");
    str.append(":");
    str.append(tokens[1]);
    return str;
}
