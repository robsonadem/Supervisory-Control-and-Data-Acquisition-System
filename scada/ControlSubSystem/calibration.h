#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <iostream>
#include <vector>
#include "../DMS/dms.h"
#include "../DMS/table.h"
/**
    Purpose: Calibration Class that stores an ID, Type, and the coeffeicients of the calibration type
            It also takes in data from DMS and calibrates it with the given the type and stores it in the DMS.

    @author Robson Adem
    @version 1.0 12/03/17
*/
using namespace std;
class Calibration
{
public:
    string calId;
    string type;
    vector<double> coeff;
    vector<double> rawData;
    vector<int> sampleIds;
    DMS* db;
    /**
      * @brief      A constructor for the Calibration Class
      * @param      DMS* db -pointer to the DMS
      */
    Calibration(DMS* db);
    /**
      * @brief      Calibrates data and stores it in the dms given a modelNumber
      * @param      string modelNumber - model Number of the calibration
      */
    void calibrate();
    /**
        * @brief     Given a string and a delimter , it splits strings.
        * @param     string s- a string to be split
        * @param     char  delimiter - a character to split the string with.
        * @return    vector<string>- split tokens
        */
    vector<string> split(const string s, char delimiter);
    /**
        * @brief     Given a string and a delimter , it splits strings.
        * @param     string s- a string to be split
        * @param     char  delimiter - a character to split the string with.
        * @return    vector<double>- split tokens turned into integer
        */
    vector<double> split_toDouble(const string s, char delimiter);

    /**
        * @brief     Given a string and a delimter , it splits strings.
        * @param     string s- a string to be split
        * @param     char  delimiter - a character to split the string with.
        * @return    vector<int>- split tokens turned into integer
        */
    vector<int> split_toInt(const string s, char delimiter);
    /**
      * @brief      Distructor
      */
    ~Calibration();

};

#endif // CALIBRATION_H
