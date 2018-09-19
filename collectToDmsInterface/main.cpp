#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    string toDB = "";
    std::ifstream infile("/Users/maxwellmcfarlane/scada_repo/CollectionDev/Collector/test.txt");
    int a;
    int b;
    string c;
    std::string d;
    std::string e;
    while (infile >> a >> b >> c >> d)

    {
        if(a == 497194){
            if(b == 3){
                e = "'twist'";
            } else if(b == 5){
                e = "'slide'";
            } else {
                e = "";
            }
        } else {
            e = "";
        }
        toDB.append(e);
        toDB.append(" ");
        toDB.append(c);
        toDB.append(" ");
        toDB.append(d);
        toDB.append(" ");
        toDB.append("\n");
    }
    cout << toDB << endl;
    cout << "Hello World!" << endl;
    return 0;
}
