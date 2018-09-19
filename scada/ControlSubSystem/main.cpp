#include <iostream>
#include "branch.h"
#include "state.h"
using namespace std;

int main(void)
{
    State s("IDLE");
    State *p=&s;
    Branch(p,"lol");

    return 0;
}
