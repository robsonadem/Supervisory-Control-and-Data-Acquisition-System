#ifndef DMSWD_H
#define DMSWD_H
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <vector>
#include "table.h"
#include "dms.h"


//file is unused
using namespace std;
/**
 * @brief The DMSWD class:This class is a watchdog class that watches the amount of accesses into a specifed table
 * then according to a flag will perform an action onto a specified database table.
 */
class DMSWD
{
public:
    DMSWD();
    //identify which table to track
    //identify the number of cycles to wait before checking conditions
    DMSWD(DMS * dms, string tableName, string lookCol, string captureCol, string value, int cycles);

    bool wait();
    vector<char*> check();
    void erase(vector<char*> k);

private:
    string tableTarget;
    string lookCol, captureCol;
    string value;
    DMS * dms;
    int cycles,dummy;

};

#endif // DMSWD_H
