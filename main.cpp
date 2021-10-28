#include <iostream>
#include <string>
#include "bridge.h"
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <algorithm>
using namespace std;

 
vector <BRDG> Bridges;
vector <LINK> Links;

string arrange(string line){
    vector <int> elements;
    for (int i=0; i<line.size()/2; i++){
        int ascii = int(char(line[2*i+1]));
        elements.push_back(ascii);
    }
    sort(elements.begin(), elements.end());
    string msg;
    for (int i=0; i<elements.size(); i++){
        msg += " " +string(1, char(elements[i]));
    }
    return msg;
}


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
            return;
        }
    }
    if (brg.RP>0) brg.ports[brg.RP-1].status = "NP";
    brg.RP=0;
}


void print(vector <int> msg){
    cout<<"|";
    for (int i=0; i< msg.size(); i++){
        cout<<msg[i];
    }
    cout<<"|";
}

bool check_end(){
    int to_check = Bridges[0].root_ID;
    for (int i = 1; i< Bridges.size(); i++){
        if (Bridges[i].root_ID != to_check) return false;
    }
    return true;
}

void clear_all_links(){
    for (int i = 0; i < Links.size(); i++){
        Links[i].clear();
    }
}

string NameArrange(string Name_array){
    return 0;
}

int main(){
    int trace, brdg;            //trace for trace flag; brdg is for number of bridges
    cin >> trace >> brdg;
    cin.ignore();
    int colon;
    string line, lan_name;
    for (int i=0; i<brdg; i++){
        getline(cin, line);
        //cout<<line;
        colon = line.find(":");
        Bridges.push_back(BRDG(stoi(line.substr(1, colon-1))));
        line = arrange(line.substr(colon+1));
        for(int j=0; j < line.length()/2; j++){
            lan_name = line[2*j + 1];
            Bridges[i].add_port(j, lan_name);
            if (!in_lan(lan_name)){
                Links.push_back(LINK(lan_name));
                //cout<<&Links[Links.size()-1]<<endl;
                
            }
            else{
                //cout<<"Already Exist "<<lan_name<<": "<<&Links[find_lan(lan_name)]<<endl;
            }
        }
    }
    for (int i=0; i<Bridges.size(); i++){
        for (int j=0; j<Bridges[i].ports.size(); j++){
            //printf("Linking port%d of B%d with lan", j+1, i+1);
            //cout<<Bridges[i].ports[j].port_lan<<endl;
            Bridges[i].ports[j].Link = &Links[find_lan(Bridges[i].ports[j].port_lan)];
        }
    }

    while (!check_end()){
        for (int i = 0; i<brdg;i++){
            if (Bridges[i].root_ID == Bridges[i].ID){
                Bridges[i].generate();
            }
        }

        for (int i = 0; i<brdg;i++){
            if (Bridges[i].root_ID != Bridges[i].ID){
                Bridges[i].forward();
                //cout<<"f "<<Bridges[i].Name<<endl;
            }
        }

        for (int i =0; i< brdg; i++){
            if (Bridges[i].root_ID == Bridges[i].ID){
                for (int j=0; j<Bridges[i].ports.size(); j++){
                    //cout<<"Checking port"<<j+1<<" of Bridge"<<i+1<<" with message: ";
                    vector <int> msg = Bridges[i].read(j+1); 
                    //print(msg);
                    //cout<<endl;
                    Bridges[i].check_update(msg, j+1);
                }
            }
            unused_brg(Bridges[i]);
        }

        clear_all_links();
        TIME++;
    }
    if (trace == 1){
        for (string each: traces){
            cout<<each<<endl;
        }
    }
    for (int i = 0; i<brdg;i++){
            cout<<Bridges[i].Name + ":";
            for (int j=0; j<Bridges[i].ports.size(); j++){
                cout<<" "<<Bridges[i].ports[j].port_lan<<"-"<<Bridges[i].ports[j].status;
            }
            cout<<endl;
        }
}

