#include "state.h"
#include "branch.h"
#include "../DMS/dms.h"
#include "../DMS/table.h"
using namespace std;
State::State (string name){
    this->name=name;
    numOfBranches=0;
    //
}


void State::loadBranch(Branch branch){
    numOfBranches++;
    branches.push_back(branch);//add the branch
}

string State::nextstate(DMS* db){

    for(int i=0;i<branches.size();i++) {
        //check if the condition is true and return the name of the branch state if true
        bool isTrue=db->controlQuery(branches[i].condition);
        if(isTrue) {return branches[i].branchState->name;}
    }
        return this->name;//return this state since none of branches have been taken
}

void State::print(){
    for(Branch b: branches){
        cout<<b.branchState->name<<",";
    }
    cout<<endl;
}

State::~State(){

}
