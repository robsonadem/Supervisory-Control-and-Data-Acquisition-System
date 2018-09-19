#include "dmswd.h"

DMSWD::DMSWD(){}

DMSWD::DMSWD(DMS *dms, string tableName, string lookCol, string captureCol, string value, int cycles){
    this->dms = dms;
    this->cycles = cycles;
    this->tableTarget = tableName;
    this->lookCol = lookCol;
    this->captureCol = captureCol;
    this->value = value;
    dummy = cycles;
}

vector<char*> DMSWD::check(){
    string s = dms->getTable(tableTarget)->createQuery(captureCol,lookCol + " > " + value);
    vector<char*> k;
    char * it;
    it = strtok((char*)s.c_str(), " ");
    while(it != NULL){
        k.push_back(it);
        it = strtok(NULL, " ");
    }
    return k;
}

void DMSWD::erase(vector<char*> k){
    for(int i = 0; i < (int)k.size() ; i++){
        dms->getTable(tableTarget)->delRow(captureCol,k.at(i));
    }
}

bool DMSWD::wait(){
    dummy--;
    if(dummy == 0){dummy = cycles; return false;}
    else{return true;}
}
