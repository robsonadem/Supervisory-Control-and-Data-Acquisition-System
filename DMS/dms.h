#ifndef DMS_H
#define DMS_H
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <vector>
#include "table.h"
#include "../tools/macros.h"
#include "../tools/baseclass.h"
#include "../tools/log.h"
#include <time.h>
#include <cstring>
#include <../scada/ControlSubSystem/modemanager.h>
#include <../scada/ControlSubSystem/branch.h>
#include <../scada/ControlSubSystem/state.h>

using namespace std;

class State;


/**
 * @brief The DMS class,This class is made to wrapper the sqlite3 database and allow the surrounding systems
 * to access the informaiton while also storing their information within the tables as well.
 * @author Maxwell McFarlane
 * @date 12/03/2017
 */
class DMS
{
public:
    DMS();
    DMS(string name);    
    DMS(string name, string dbConfig);

    //This consrtuctor is the preferred constructor , specifies name, configuration of tables, and logging path
    DMS(string name, string dbConfig, string logPath);

    //methods allow the user to create a table
    void createTable(string tableName);
    void createTable(string tableName, string dim);    

    //returns table
    Table* getTable(string tableName);
    //removes table
    void dumpTable(string tableName);
    //clears table
    void clearTable(string tableName);
    //closes database
    void close();
    //returns the log
    Log* getLog();
    //changes the log used
    void getNewLog();

    //delimitter for commands
    vector<char*> delimitter(string cmd);    

    //query method that returns whether a condition is true or false
    bool controlQuery(string cmd);
    //returns the headers of a table
    string getTableHeaders(string tableName);
    //returns the list of tables
    vector<Table*> getTableList();

    //loads .db with testbench file
    void loadDataBase(string  myfile);
    //loads .db with config files
    void loadConfigTable(string fileName , string myfile);
    //shows if sensorExists in .db
    bool isSensorExist(string sensorName);    

    //resets row ids after data deletion
    void resetRowIdTable(string tableName, string col);
    void setCurrentState(string  currentState);
    string getCurrentState();

private:

    //callback methods
    static int cbDropTable           (void *data, int argc, char **argv, char **azColName);
    static int cbgetTableHeaders     (void *data, int argc, char **argv, char **azColName);
    static int cbSize          (void *data, int argc, char **argv, char **azColName);
    //used to manipulate table with correct info
    vector <Table*> sensList;
    string filename;
    Table lookup;
    sqlite3 *db;
    Log * log = new Log("../error_files/dms_log.txt");

    static int n;
    string currentState;

};

#endif // DMS_H
