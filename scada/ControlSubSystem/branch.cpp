#include <iostream>
using namespace std;
#include "branch.h"
#include "state.h"

Branch::Branch (State *branchState, string condition ){
    this->branchState= branchState;
    this->condition=condition;
}

Branch::~Branch(){

}

