#ifndef STATE_H
#define STATE_H
#include <iostream>
#include <vector>
#include "branch.h"
#include "../DMS/dms.h"
#include "../DMS/table.h"
/**
    Purpose: A State Class that stores the name and the branches of a state

    @author Robson Adem
    @version 1.0 12/03/17
*/
using namespace std;

class Branch;
class DMS;

class State{
public:
    string name;
    vector<Branch> branches;
    int numOfBranches;
    /**
         * @brief      A constructor for the State Class.
         * @param      string name- the name of the state.
         * */

    State(string name);
    /**
        * @brief
        * @param      Branch branch - Branches of the state.
        */
    void loadBranch(Branch branch);
    /**
        * @brief     Prints all the possible nextstates.
        */
    void print();
    /**
        * @brief     Given a DMS pointer it pick which branch takes place and returns the name of the next state.
        * @param     DMS * db - a pointer to the DMS.
        * @return    string -  the name of the next state.
        */
    string nextstate(DMS *db);
    /**
        * @brief     Distructor
        */
    ~State();

};

#endif // STATE_H
