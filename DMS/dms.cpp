#include "dms.h"

//constructors
DMS::DMS()
{
    time_t now = time(0);
    string dt = ctime(&now);

    *log << "\n" << dt;
    *log << "Error: No database given\n";
}

DMS::DMS(string name, string dbConfig){
    int rc = 0;
    char *ermsg = 0;
    time_t now = time(0);
    string dt = ctime(&now);

    *log << "\n" << dt;

    filename = name.c_str();
    rc = sqlite3_open(name.c_str(),&db);
    //allow system to accept foreign keys
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &ermsg);
    rc = sqlite3_exec(db, ".separator |", 0, 0, &ermsg);
    //    *log << (string)ermsg << "\n";
    if(rc == SQLITE_OK){
        *log << "DataBase " << filename << " is opened.\n";
    }
    else{
        *log <<"Error: " << rc << " has occured.\n";
    }

    string tableName,dimensions,dummy;
    ifstream myconfig(dbConfig);

    if(myconfig.is_open()){
        while(!myconfig.eof()){
            //searches for the next chunk of text
            while(tableName.empty()){getline(myconfig,tableName);}
            getline(myconfig,tableName);
            //getState the list of dimensions
            while(dummy != "*"){dimensions += dummy;getline(myconfig,dummy);}
            //            getline(myconfig,dimensions);
            createTable(tableName,dimensions);
            getline(myconfig,dimensions);
            getTable(tableName)->setDimensions(dimensions);
        }
        myconfig.close();
    }
    else{*log << "Unable to open file.\n"; cerr << "Unable to open file.\n";}

}

DMS::DMS(string name){
    int rc = 0;
    char *ermsg = 0;
    //current time
    time_t now = time(0);
    string dt = ctime(&now);

    *log << "\n" << dt;

    filename = name.c_str();
    rc = sqlite3_open(name.c_str(),&db);
    //allow system to accept foreign keys
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &ermsg);
    rc = sqlite3_exec(db, ".separator |", 0, 0, &ermsg);
    //    *log << (string)ermsg << "\n";

    if(rc == SQLITE_OK){
        *log << "DataBase " << filename << " is opened.\n";
    }
    else{
        *log <<"Error: " << rc << " has occured.\n";
    }
}

DMS::DMS(string name, string dbConfig, string logPath){    
    int rc = 0;
    char *ermsg = 0;
    log = new Log(logPath);
    time_t now = time(0);
    string dt = ctime(&now);

    *log << "\n" << dt;

    filename = name.c_str();
    ifstream dbcheck("../scada.db");
    bool fExist = dbcheck.good();

    rc = sqlite3_open(name.c_str(),&db);

    //    if(!fExist){
    //        this->loadDataBase("../testbench_files/StateTableTB.txt");
    //        this->loadDataBase("../testbench_files/BranchTableTB.txt");
    //        this->loadDataBase("../testbench_files/ConditionTableTB.txt");

    //        this->loadDataBase("../testbench_files/SensorTableTB.txt");
    //        this->loadDataBase("../testbench_files/SensorConfTableTB.txt");
    //        this->loadDataBase("../testbench_files/CalConfTableTB.txt");

    //        this->loadDataBase("../testbench_files/SampleTableTB.txt");
    //        this->loadDataBase("../testbench_files/CalibrationSampleTableTB.txt");

    //        this->loadConfigTable("ControlState_config.txt","/Users/maxwellmcfarlane/scada_repo/configuration_files/ControlState_config.txt");
    //    }

    //allow system to accept foreign keys
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &ermsg);
    //    *log << ermsg << "\n";
    if(rc == SQLITE_OK){
        *log << "DataBase " << filename << " is opened.\n";
    }
    else{
        *log <<"Error: " << rc << " has occured.\n";
    }
    createTable("ConfigFileTable","(fileName TEXT PRIMARY KEY, contents TEXT);");
    getTable("ConfigFileTable")->setDimensions("(Configuration)");
    string tableName,dimensions,dummy;
    ifstream myconfig(dbConfig);

    if(myconfig.is_open()){
        while(!myconfig.eof()){
            //searches for the next chunk of text
            while(tableName.empty()){getline(myconfig,tableName);}
            //getState the list of dimensions
            while(dummy != "*"){dimensions += dummy;getline(myconfig,dummy);}
            //creates table
            createTable(tableName,dimensions);
//            getTable("ConfigFileTable")->addToTable("'" + tableName + " " + dimensions + "'");
            getline(myconfig,dimensions);
            //sets dimensions to a usable format
            getTable(tableName)->setDimensions(dimensions);
            //reset parsing variables
            tableName = "";
            dimensions = "";
            dummy = "";
        }
        myconfig.close();
    }
    else{*log << "Unable to open file.\n"; cerr << "Unable to open file.\n";}
}

//methods
/**
 * @brief DMS::createTable, creates a table based on a name
 * @param name
 */
void DMS::createTable(string name){
    Table * t = new Table(name,db,log);
    sensList.push_back(t);
}

/**
 * @brief DMS::createTable, creates table based on namd and dim
 * @param name
 * @param dim
 */
void DMS::createTable(string name, string dim){
    Table * t = new Table(name,dim, db,log);
    sensList.push_back(t);
}

/**
 * @brief DMS::getTable, returns table from table list vector
 * @param tableName
 * @return
 */
Table* DMS::getTable(string tableName){
    for(int i = 0; i < (int)sensList.size(); i++){
        Table * t = sensList.at(i);
        if(tableName == t->getTableName()){return t;}
    }
    return nullptr;
}

/**
 * @brief DMS::getTableList, returns the table list vector
 * @return
 */
vector<Table*> DMS::getTableList(){
    return sensList;
}

/**
 * @brief DMS::dumpTable, removes table from database
 * @param tableName
 */
void DMS::dumpTable(string tableName){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    string cmd = "DROP TABLE ";
    cmd += tableName;
    cmd += ";";
    Table * t;
    sql = cmd.c_str();
    for(int i = 0; i < (int)sensList.size(); i++){
        t = sensList.at(i);
        if(tableName == t->getTableName()){
            rc = sqlite3_exec(db,sql,cbDropTable,(void*)log,&ermsg);
            //            *log << (string)ermsg << "\n";
            break;
        }
    }

    if(rc != SQLITE_OK){
        *log << "Error: " << rc << " Table: " << t->getTableName() << " wasn't be deleted.\n";
    }
    else{
        *log << "Table: " << t->getTableName() << " was deleted.\n";
    }
}

/**
 * @brief DMS::delimitter, delimits string based on commas for .db entries
 * @param cmd
 * @return
 */
vector<char*> DMS::delimitter(string cmd){
    vector<char*> k;
    char * it;
    it = strtok((char*)cmd.c_str(), ",");
    while(it != NULL){
        k.push_back(it);
        it = strtok(NULL, ",");
    }

    return k;
}

/**
 * @brief DMS::controlQuery, a specified method used by the control subsystem
 * that allows the subsystem to determine if a condition has been met.
 * @param cmd
 * @return
 */
bool DMS::controlQuery(string cmd){    
    string sql,sensorName,tableName;
    //delimitter
    vector<char*> list;
    char * t;
    t = strtok((char*)cmd.c_str(), ": ");
    while(t != NULL){
        list.push_back(t);
        t = strtok(NULL, ": ");
    }
    //specified format data acquisition
    tableName = (string)list.at(4);
    sensorName = (string)list.at(0);
    for(int i = 1; i < (int)list.size(); i++){sql += (string)list.at(i) + " " ;}
    sql += " And SensorId = " + sensorName+ ";";    
    if((string)list.at(0) != "null"){
        if(!getTable(tableName)->isQueryEmpty(sql)){return true;}
    }
    else{cerr << "Err: Incorrect Format for Query Condition." << endl;}
    return false;
}

//returns the log of the system
Log* DMS::getLog(){return log;}

//refreshes the log
void DMS::getNewLog(){log = new Log();}

/**
 * @brief DMS::loadDataBase, loads testbench data into the .db
 * @param myfilePath
 */
void DMS::loadDataBase(string myfilePath){
    ifstream myfile(myfilePath);
    string tableName;
    string data;
    if(myfile.is_open()){
        getline(myfile,tableName);
        if(getTable(tableName) != 0){
            while(!myfile.eof()){
                getline(myfile,data);
                getTable(tableName)->addToTable(data);
            }
        }
        else{*log << "Table Retrieval Error.\n";}
        myfile.close();
    }
    else{*log << "Unable to load data.\n"; cerr << "Unable to open file.\n";}
}

/**
 * @brief DMS::loadConfigTable, loads and updates confguraiton files stored in .db
 * @param fileName
 * @param myfilePath
 */
void DMS::loadConfigTable(string fileName, string myfilePath){
    std::ifstream ifs(myfilePath);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );
    string cmd = "insert into configfiletable(filename,contents) values('"+ fileName +"', \"" + content +"\" );";
    int rc;
    char * zErrmsg;
    rc = sqlite3_exec(db,cmd.c_str(),0,0,&zErrmsg);
    if(rc != SQLITE_OK){
//        cout << endl;
//        cout << zErrmsg << endl;
        string cmd = "Update configfiletable Set contents = '"+ content +"' where filename = '"+ fileName +"';";
//        cout << cmd << endl;
        sqlite3_exec(db,cmd.c_str(),0,0,&zErrmsg);
//        cout << zErrmsg << endl;
    }
}

//closes the .db
void DMS::close(){
    sqlite3_close(db);
    //current time
    time_t now = time(0);
    string dt = ctime(&now);

    *log << dt;
}


/**
 * @brief DMS::getTableHeaders,
 * @param tableName
 * @return
 */
string DMS::getTableHeaders(string tableName){
    string headers;
    char* ermsg;
    string cmd = "Select * from " + tableName;
    const char* sql = cmd.c_str();
    sqlite3_exec(db,sql, cbgetTableHeaders, (void *)&headers, &ermsg);
    n = 1;
    //    *log << (string)ermsg << "\n";
    return headers;
}

bool DMS::isSensorExist(string sensorName){
    string cmd = "select sensorid from sensortable where sensorid = " + sensorName + ";";
    char * ermsg;
    int rc;
    rc = sqlite3_exec(db,cmd.c_str(), 0, 0, &ermsg);
    //    *log << (string)ermsg << "\n";
    if(rc != SQLITE_OK){
        return false;
    }
    else{return true;}
}

void DMS::setCurrentState(string currentState){this->currentState = currentState;}
string  DMS::getCurrentState(){return currentState;}

//Callback Functions
int DMS::cbDropTable(void *data, int argc, char **argv, char **azColName){
    Log * log0 = (Log *) data;
    int i;
    for(i = 0; i < argc; i++){*log0 << azColName[i] << " ";}
    *log0 << "\n";
    (void)argv;
    return 0;
}

int DMS::cbgetTableHeaders(void *data, int argc, char **argv, char **azColName){
    string * headers = (string *) data;
    if(n == 1){
        for(int i = 0; i < argc; i++){
            *headers += azColName[i];
            if(i < argc-1){*headers += " | ";}
        }
        n--;
    }
    (void)argc;
    (void)argv;
    return 0;
}

int DMS::n = 1;
void DMS::clearTable(string tableName){

    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    string cmd = "DELETE FROM ";
    cmd += tableName;
    cmd += ";\n VACUUM;";

    sql = cmd.c_str();

    rc = sqlite3_exec(db,sql,0,log, &ermsg);

    if(rc != SQLITE_OK){
        *log << "Error: " << rc;
        *log << tableName << ": Row couldn't be deleted.\n";
    }
    else{
        *log << tableName << "clear corruption.\n";
    }


}


//void DMS::resetRowIdTable(string tableName,string col){
//    string cmd,cmd1,cmd2;
//    int rc;
//    string maxIndex,minIndex;
//    int index,sqindex;
//    cmd1 = "select Max(rowid) from "+tableName + ";";
//    cmd2 = "select Min(rowid) from "+tableName + ";";
//    rc = sqlite3_exec(db,cmd1.c_str(),cbSize,(void *) &maxIndex, 0);
//    index = stoi(maxIndex);
//    rc = sqlite3_exec(db,cmd2.c_str(),cbSize,(void *) &minIndex, 0);
//    sqindex = stoi(minIndex);
//    int j = 1;
//    while(j != index){
//        cmd = "UPDATE "+tableName+" SET "+ col + "= "+ to_string(j) +" WHERE " + col + "= "+ to_string(sqindex) +";";
//        cout << cmd << endl;
//        //        cout << !getTable(tableName)->isQueryEmpty("select rowid from "
//        //                                                   + tableName
//        //                                                   + " where " +col+ "="
//        //                                                   +to_string(j) + ";") << endl;
//        rc = sqlite3_exec(db,cmd.c_str(),0,0, 0);
//        if(!getTable(tableName)->isQueryEmpty("select rowid from "
//                                              + tableName
//                                              + " where " +col+ "="
//                                              +to_string(j) + ";")){sqindex++;}
//        j++;
//    }
//}


//void DMS::resetRowIdTable(string tableName,string col){
//    string cmd,cmd1,cmd2;
//    int rc;
//    string maxIndex,minIndex;
//    int index,sqindex;
//    cmd1 = "select Max(rowid) from "+tableName + ";";
//    cmd2 = "select Min(rowid) from "+tableName + ";";
//    rc = sqlite3_exec(db,cmd1.c_str(),cbSize,(void *) &maxIndex, 0);
//    index = stoi(maxIndex);
//    rc = sqlite3_exec(db,cmd2.c_str(),cbSize,(void *) &minIndex, 0);
//    sqindex = stoi(minIndex);
//    int j = 1;
//    while(j != index){
//        cmd = "UPDATE "+tableName+" SET "+ col + "= "+ to_string(sqindex) +" WHERE " + col + "= "+ to_string(j) +";";
////                                                   + tableName
////                                                   + " where " +col+ "="
////                                                   +to_string(j) + ";") << endl;
//        rc = sqlite3_exec(db,cmd.c_str(),0,0, 0);
//        if(!getTable(tableName)->isQueryEmpty("select rowid from "
//                                             + tableName
//                                             + " where " +col+ "="
//                                             +to_string(j) + ";")){sqindex++;}
//        j++;
//    }
//}


int DMS::cbSize(void *data, int argc, char **argv, char **azColName){
    string * number = (string *) data;
    *number = (string)*argv;
    (void)argc;
    (void)azColName;
    return 0;
}



