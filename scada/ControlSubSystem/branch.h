#ifndef BRANCHES_H
#define BRANCHES_H
#include <iostream>
#include "state.h"
/**
    Purpose: Branch Class that stores a state to branch to and the condition for it

    @author Robson Adem
    @version 1.0 12/03/17
*/
using namespace std;

class State;

class Branch{
public:
    State *branchState;
    string condition;

      /**
        * @brief      A constructor for the Branch Class
        * @param      State* branchState - a  pointer to a state to branch to
        * @param      string condition - a condiiton to be fullfiled for the branch to happen
        */
    Branch (State* branchState, string condition );
      /**
        *  Distructor
        */
    ~Branch();
};


#endif // BRANCHES_H
