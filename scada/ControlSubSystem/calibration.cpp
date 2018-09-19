#include "calibration.h"
#include <time.h>
#include <math.h>
Calibration::Calibration( DMS*db)
{

//    string sensors=db->getTable("CalConfTable")->createQuery("select sensorId from calconftable where stateName = '"
//                                                                            + db->getCurrentState()
//                                                                            +"' and isCal = 0;");
    string calibrationDeclaration=db->getTable("CalConfTable")->createQuery("select calibration from calconftable where stateName = '"
                                                                            + db->getCurrentState()
                                                                            +"' and isCal = 0;");

    cout<<calibrationDeclaration;
    vector<string> calibrationLine=split(calibrationDeclaration,',');
    this->type=calibrationLine[0];
    this->coeff=split_toDouble(calibrationLine[1],':');
    this->db=db;
}

Calibration::~Calibration()
{

}

void  Calibration::calibrate()
{
    //get data form DMS
    
    string returnedRawDataString= db->getTable("SampleTable")->createQuery("select RAWDATA from sampletable where ");
    string returnedSampleIdString=db->getTable("SampleTable")->createQuery("SAMPLEID","");
    string modelNumber= db->getTable("ConfCalTable")->createQuery("select ModelNumber from calconftable where stateName = '");
    rawData=split_toDouble(returnedRawDataString,'\n');//split lines
    sampleIds= split_toInt(returnedSampleIdString,'\n');//split lines
    double calVal;
    for(int i=0; i<rawData.size();i++){
        //CALIBRATE IT
        if(!type.compare("linear"))calVal= rawData[i]*coeff[0]+ coeff[1];
        else if (!type.compare("quad")) calVal= (pow(rawData[i],2))*coeff[0]+ rawData[i]*coeff[1]+ coeff[2];
        else break;
        cout<<to_string(calVal)<<endl;
        struct timespec tv;
            //gettimeofday(&tv, NULL);
            // use CLOCK_MONOTONIC for systems that want a time that will not be adjusted
            clock_gettime(CLOCK_REALTIME, &tv);
            unsigned long long millisecondsSinceEpoch =
                    (unsigned long long)(tv.tv_sec) * 1000 +
                    (unsigned long long)(tv.tv_nsec) / 1000000;
        //PUT IT all BACK

        db->getTable("CalibratedSampleTable")->addToTable(to_string(sampleIds[i]) +","+to_string(millisecondsSinceEpoch)+","+ to_string(calVal) + ","+ modelNumber);

    }
}

vector<double> Calibration::split_toDouble(const string s, char delimiter)
{
    vector<double> tokens;
    string token;
    istringstream tokenStream(s);
    double tokenInt;
    while (getline(tokenStream, token, delimiter))
    {
        tokenInt= stod(token);//change string into integer
        tokens.push_back(tokenInt);
    }
    return tokens;
}

vector<int> Calibration::split_toInt(const string s, char delimiter)
{
    vector<int> tokens;
    string token;
    istringstream tokenStream(s);
    int tokenInt;
    while (getline(tokenStream, token, delimiter))
    {
        tokenInt= stoi(token);//change string into integer
        tokens.push_back(tokenInt);
    }
    return tokens;
}

vector<string> Calibration::split(const string s, char delimiter)
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

