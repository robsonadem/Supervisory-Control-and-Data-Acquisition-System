#include "table.h"

Table::Table(){}

Table::Table(string tableName, string dim, sqlite3 *db, Log *log){    
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    this->tableName = tableName;
    this->dimensions = dim;

    this->db = db;
    string cmd;
    this->log = log;
    cmd += "CREATE TABLE ";
    cmd += tableName + " " + dim;
    sql = cmd.c_str();
    rc = sqlite3_exec(db,sql,cbCreateTable,0, &ermsg);    
    //    *log << (string)ermsg << "\n";

    if(rc != SQLITE_OK){
        *log << "Error: " << rc << "\n";
        if(rc == 1){*log << "Table:" << tableName << " already exists.\n";}
        *log << tableName << " could not be created.\n";
    }
    else{
        *log << tableName << " was created.\n";
        *log << "Dimensions: " << dim << "\n";
    }
    tableLength = 0;
}

Table::Table(string tableName, sqlite3 *db, Log * log){    
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    this->tableName = tableName.c_str();
    this->db = db;

    this->log = log;
    string cmd;

    if(tableName == "HubTable"){
        cmd = "CREATE TABLE ";
        cmd += tableName;
        cmd +=
                "(HubSN TEXT PRIMARY KEY  NOT NULL,"
                "NumberOfSensors TEXT NOT NULL);";
    }
    else if(tableName == "SampleTable"){
        cmd = "CREATE TABLE ";
        cmd += tableName;
        cmd += " (Sample TEXT PRIMARY KEY NOT NULL, "
               "SensorID TEXT NOT NULL, "
               "RawData TEXT NOT NULL, "
               "CalData TEXT);";
    }
    else if(tableName == "CalibrationTable"){
        cmd = "CREATE TABLE ";
        cmd += tableName;
        cmd += " (CalIndex INT PRIMARY KEY NOT NULL, "
               "Sample BLOB NOT NULL, "
               "ModelNumber BLOB NOT NULL, "
               "CalibrationTimeStamp BLOB NOT NULL, "
               "CalibrationValue BLOB NOT NULL);";
    }
    sql = cmd.c_str();
    rc = sqlite3_exec(db,sql,cbCreateTable,0, &ermsg);
    //    *log << (string)ermsg << "\n";

    if(rc != SQLITE_OK){
        *log << "Error: " << rc << "\n";
        if(rc == 1){*log << "Table:" << tableName << " already exists.\n";}
        *log << tableName << " could not be created.\n";
    }
    else{
        *log << tableName << " was created.\n";
        *log << "Dimensions: " << cmd << "\n";
    }
    tableLength = 0;
}

string Table::getTableName(){return tableName;}
int Table::getTableLength(){return tableLength;}

//table methods
void Table::addToTable(string info){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    string cmd = "INSERT INTO ";
    cmd+= tableName;
    cmd+= " " + dimensions;
    cmd+=" VALUES(";
    cmd += info;
    cmd += ");";
    sql = cmd.c_str();
//    cout << cmd << endl;
    rc = sqlite3_exec(db,sql,cbAddToTable,0, &ermsg);

    if(rc != SQLITE_OK){        
        *log << "Error: " << rc << " in " << tableName << "\n";
        if(rc == 19){*log << "Item: " << info << " caused a constraint violation.\n";}
        *log << " Item could not be inserted.\n";
    }
    else{
        *log << tableName << ": Item was inserted.\n";
        tableLength++;
    }
}
void Table::addMultiToTable(string info){
    vector<char*> list;
    char * it;
    it = strtok((char*)info.c_str(), "\n");
    while(it != NULL){
        list.push_back(it);
        it = strtok(NULL, "\n");
    }
    for(int i = 0; i < (int)list.size(); i++){
        addToTable(list.at(i));
    }
}

void Table::delRow(string col, string index){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    string cmd = "DELETE FROM ";
    cmd += tableName;
    cmd += " WHERE ";
    cmd += col;
    cmd += " = ";
    cmd += index;
    cmd += ";";
    sql = cmd.c_str();

    rc = sqlite3_exec(db,sql,cbDelRow,log, &ermsg);

    *log << (string)ermsg << "\n";

    if(rc != SQLITE_OK){
        *log << "Error: " << rc;
        *log << tableName << ": Row couldn't be deleted.\n";
    }
    else{
        *log << tableName << " row[ " << index << "] was deleted.\n";
        tableLength--;
    }
}

string Table::createQuery(string cmd){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;

    sql = cmd.c_str();

    string result;
    rc = sqlite3_exec(db,sql,cbCreateQuery,&result, &ermsg);
    //    *log << (string)ermsg << "\n";

    if(rc == SQLITE_ERROR){
        *log << "Error: " << rc;
        *log << tableName << " was accessed.\n";
        *log << "Query wasn't created.\n";
    }
    else{
        *log << "Query created.\n";
    }

    return result;
}

string Table::createQuery(string col, string op){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;

    string cmd = "SELECT ";
    cmd += col;
    cmd += " FROM ";
    cmd += tableName;
    if(!op.empty()){
        cmd += " WHERE ";
        cmd += op;
    }
    cmd += ";";
    sql = cmd.c_str();

    string result;
    rc = sqlite3_exec(db,sql,cbCreateQuery,&result, &ermsg);
    //    *log << (string)ermsg << "\n";

    if(rc == SQLITE_ERROR){
        *log << "Error: " << rc;
        *log << tableName << " was accessed.\n";
        *log << "Query wasn't created.\n";
    }
    else{
        *log << "Query created.\n";
    }

    return result;
}

void Table::exp(string col, string op, string filePath){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;

    string cmd = "SELECT ";
    cmd += col;
    cmd += " FROM ";
    cmd += tableName;
    if(!op.empty()){
        cmd += " WHERE ";
        cmd += op;
    }
    cmd += ";";
    sql = cmd.c_str();

    ofstream myfile(filePath);
    string result;
    headerN++;
    rc = sqlite3_exec(db,sql,cbExp,(void*)&result, &ermsg);
    //    *log << (string)ermsg << "\n";

    myfile << result;
    myfile.close();

    if(rc == SQLITE_ERROR){
        *log << "Error: " << rc;
        *log << tableName << " was accessed.\n";
        *log << "...\n";
        cerr << "no export\n";
    }
    else{
        cerr << "export\n";
        *log << "export.\n";
    }

}

void Table::updateTable(string index, string op){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;

    string cmd = "UPDATE ";
    cmd += tableName;
    cmd += " SET ";
    cmd += op;
    cmd += " WHERE ";
    cmd += index;
    cmd += ";";
    sql = cmd.c_str();
    string result;
    rc = sqlite3_exec(db,sql,cbUpdate,&result, &ermsg);

    if(rc == SQLITE_ERROR){
        *log << "Error: " << rc;
        *log << tableName << " attempted access.\n";
        *log << "Couldn't update.\n";
    }
    else{
        *log << tableName << " was updated.\n";
    }
}

void Table::setDimensions(string newD){dimensions = newD;}

bool Table::isQueryEmpty(string cmd){
    string s = createQuery(cmd);
    if(s.empty()){return true;}
    else{return false;}
}

vector<char*> Table::delimitter(string cmd){
    vector<char*> k;
    char * it;
    it = strtok((char*)cmd.c_str(), " ");
    while(it != NULL){
        k.push_back(it);
        it = strtok(NULL, " ");
    }

    return k;
}

int Table::count(){
    int rc = 0;
    char *ermsg = 0;
    const char * sql;
    string size;
    string cmd = "SELECT count(*) from ";
    cmd+= tableName;
    cmd += ";";
    sql = cmd.c_str();

    rc = sqlite3_exec(db,sql,cbSize,(void *)&size, &ermsg);
    return stoi(size);
}
//callback methods

int Table::cbCreateTable(void *data, int argc, char **argv, char **azColName){
    Log * log0 = (Log *)data;
    if(argc < 1){
        *log0 << "Table couldn't be created\n";
    }
    else{
        *log0 << "Table could be created.\n";
    }
    (void)argv;
    (void)azColName;
    return 0;
}

int Table::cbAddToTable(void *data, int argc, char **argv, char **azColName){
    Log * log0 = (Log *)data;
    if(argc < 1){
        *log0 << "Couldn't add to table.\n";
    }
    else{
        *log0 << "Data has been added to table.\n";
    }
    (void)argv;
    (void)azColName;
    return 0;
}

int Table::cbDelRow(void *data, int argc, char **argv, char **azColName){
    Log * log0 = (Log *)data;
    if(argc < 1){
        *log0 << "Row couldn't be deleted.\n";
    }
    else{
        *log0 << "Row was deleted.\n";
    }
    (void)argv;
    (void)azColName;
    return 0;
}

int Table::cbCreateQuery(void *data, int argc, char **argv, char **azColName){
    int i;
    string * result = (string *) data;
    for(i = 0; i < argc; i++){
        if(argv[i] != NULL){*result += argv[i];}
        if(i < argc-1){*result += "|";}}
    *result += "\n";
    (void)azColName;
    return 0;
}

int Table::cbUpdate(void *data, int argc, char **argv, char **azColName){
    Log * log0 = (Log *)data;
    if(argc < 1){
        *log0 << "Table was not updated.\n";
    }
    else{
        *log0 << "Table was deleted.\n";
    }
    (void)argv;
    (void)azColName;
    return 0;
}

int Table::cbExp(void *data, int argc, char **argv, char **azColName){
    string * myfile = (string*)data;

    //prints out columns
    if(headerN == 1){
        for(int i = 0; i < argc; i++){
            if(argv[i] != NULL){*myfile += azColName[i];}
            if(i < argc-1){*myfile += ",";}
        }
        *myfile += "\n";
        headerN--;
    }
    for(int i = 0; i < argc; i++){
        if(argv[i] != NULL){*myfile += argv[i];}
        if(i < argc-1){*myfile +=  ",";}
    }
    *myfile += "\n";
    return 0;
}

int Table::cbSize(void *data, int argc, char **argv, char **azColName){
    string * number = (string *) data;
    *number = (string)*argv;
    (void)argc;
    (void)azColName;
}

int Table::headerN = 0;
