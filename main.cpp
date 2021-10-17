#include <iostream>
#include <string>
#include "bridge.h"
//#include "bridgesim.h"
#include <vector>
#include <string>
#include <cstring>


using namespace std;


unsigned int TIME=0;

bool in_lan(string lan_name){
    for (int i=0; i < Links.size(); i++){
        if (Links[i].LAN == lan_name) return true;
    }
        return false;
}

int find_lan(string lan_name){
    if (in_lan(lan_name)){
        for (int i=0; i < Links.size(); i++){
            if (Links[i].LAN == lan_name){ 
                return i;
            }
        }
        return -1;
    }
    else return -1;
}

void unused_brg(BRDG &brg){
    for (int i=0; i<brg.ports.size(); i++){
        if (brg.ports[i].status == "DP"){
            continue;
        }
    }
    brg.ports[brg.RP].status = "NP";
}


int main()
{
    // BRDG brg(1);
    // PORT port(0);
    // LINK link("A");
    // port.linking(link);
    // brg.ports.push_back(port);
    // cout<<port.Link->LAN;
    // vector <int> msg{1, 5, 3};
    // brg.write(msg, 1);

    // vector <int> reply = brg.read(1);
    // for (int i=0; i<3; i++){
    //     cout<< reply[i]<<" ";
    // }


    int trace, brdg;            //trace for trace flag; brdg is for number of bridges
    cin >> trace >> brdg;
    cin.ignore();
    for (int i=0; i<brdg; i++){
        string line;
        getline(cin, line);
        int colon = line.find(":");
        BRDG brd(stoi(line.substr(1, colon-1)));
        for(int j=0; j < line.substr(colon+1).length()/2; j++){
            string lan_name = line.substr((colon + 1) + (2*j + 1), 1);
            if (!in_lan(lan_name)){
                PORT port(j);
                LINK link(lan_name);
                port.linking(link);
                brd.ports.push_back(port);
            }
            else{
                PORT port(j);
                port.linking(Links[find_lan(lan_name)]);
                brd.ports.push_back(port);
            }
        }
        Bridges.push_back(brd);
        }




        for (int i = 0; i<brdg;i++){
            if (Bridges[i].root_ID == Bridges[i].ID){
                Bridges[i].generate();
            }
        }
        for (int i = 0; i<brdg;i++){
            if (Bridges[i].root_ID != Bridges[i].ID){
                Bridges[i].forward();
                cout<<"f "<<Bridges[i].Name<<endl;
            }
        }

        for (int i =0; i< brdg; i++){
            for (int j=0; j<Bridges[i].ports.size(); j++){
                if (Bridges[i].root_ID == Bridges[i].ID){
                    Bridges[i].check_update(Bridges[i].read(j), j);
                }
            }
            unused_brg(Bridges[i]);
        }
}

