 #define MAX_BRIDGES

#include <iostream>
#include <string>
#include <vector>
#include <valarray>


using namespace std;

#ifndef CLASSES_H
#define CLASSES_H

class BRDG;
class PORT;
class LINK;



vector <BRDG> Bridges;
vector <LINK> Links;




//////////////////////////EASE///////////////////////////


vector <int> filter(vector <vector <int>> msg){                //make vector of vector a single vector
    vector <int> filtered;
    for (int i=0; i< msg.size(); i++){
        filtered.insert(filtered.end(), msg[i].begin(), msg[i].end());
    }
    return filtered;
}

vector <vector <int>> slicing(vector <vector <int>> msg, int x){
    msg.erase(msg.begin()+x);
    return msg;
}
//////////////////////PORT//////////////////////////////


class PORT{
    public:
        string status;
        int ID;
        LINK* Link;
        PORT(int id){
            ID = id+1;
            status = "DP";
        }
        void linking(LINK &link){
            Link = &link;
            }
        void status_update(string update){
            status = update;
        }
};

////////////////////LINK/////////////////////////////


class LINK{
    public:
        int total_msg;
        string LAN;
        LINK(string lan_to_be_connected){
            total_msg = 0;
            LAN = lan_to_be_connected;
        }
        vector <int> Data;
        void load(vector <int> msg);
        void clear();
        vector <int> accept();
};


void LINK::load(vector <int> msg){
    for (int i=0; i<msg.size(); i++){
        Data.push_back(msg[i]);
    }
    total_msg += msg.size()/3;
}

void LINK::clear(){
    Data.clear();
    total_msg = 0;
}

vector <int> LINK::accept(){
    vector <int> data;
    data = Data;
    clear();
    return data;
}


/////////////////////////////BRIDGES/////////////////////////////////////

class BRDG{
    public:
        int ID;                                     //ID of the bridge in integers. Bridge with ID 1 would be ROOT Bridge 
        string Name;                                //Name of the bridge e.g. Bxx : xx is the ID
        vector <PORT> ports;
        vector <vector <int>> BUFFER;
        vector <int> NonNull;
        int RP;
        int rootDis;
        int root_ID;
        int sending_brg;


        BRDG(int id){
            ID = id;
            Name = "B" + to_string(id);
            rootDis = 0;
            root_ID = id;
            sending_brg = id;
        }

        void write(vector <int> message, int port);
        vector <int> read(int port);
        void forward();
        void generate();
        void check_update(vector <int> msg, int msg_port);              
};



void BRDG::check_update(vector <int> msg, int msg_port){
    for(int i = 0; i < msg.size()/3; i++){
        cout<<"r "<<Name<<" "<< "(B" + to_string(msg[i]) + ", " + to_string(msg[i+1])+", B" + to_string(msg[i+2]) + ")    port: "<<to_string(ports[i].ID)<<endl;
        if (msg[i] < root_ID){
            root_ID = msg[i];
            rootDis = msg[i+1] + 1;
            sending_brg = msg[i+2];
            RP = msg_port;
        }
        else if (msg[i] == root_ID  && msg[i+1] + 1 < rootDis){
            rootDis = msg[i+1] + 1;
            sending_brg = msg[i+2];
            RP = msg_port;
        }
        else if (msg[i] == root_ID && msg[i+1] + 1 == rootDis && sending_brg <msg[i+2]){
            sending_brg = msg[i+2];
            RP = msg[i+1];
        }
        else if (msg[i] == root_ID && msg[i+1] +1 == rootDis && sending_brg == msg[i+2] && msg_port != RP){
            ports[msg_port].status = "NP";
            NonNull.erase(find(NonNull.begin(), NonNull.end(), msg_port));
        }
    }
}



void BRDG::write(vector <int> message, int port){
    ports[port-1].Link->load(message);
}


vector <int> BRDG::read(int port){                //reading clears the link
    return ports[port-1].Link->accept();
}


void BRDG::generate(){
    vector <int> msg{ID, 0, ID};
    for (int i=0; i<ports.size(); i++){
        write(msg, i+1);
        cout<<"s "<<Name<<" "<< "(" + Name + ", 0, " + Name + ")    port: "<<to_string(ports[i].ID)<<endl;
    }
}

void BRDG::forward(){
    for (int i=0; i < NonNull.size(); i++){
        vector <int> readed = read(NonNull[i]);             //link is empty after read();
        if (readed.empty()){
            vector <int> null_msg{0, 0, 0};
            BUFFER.push_back(null_msg);
        }
        else{
            BUFFER.push_back(readed);
        }
    }
    for (int i=0; i<NonNull.size(); i++){                   //update with gathered information
        check_update(BUFFER[i], NonNull[i]);
    }
    for (int i=0; i < NonNull.size(); i++){
        ports[NonNull[i]].Link->load(filter(slicing(BUFFER, i)));
    }
}






///////////////////////////LAN////////////////////////


class LAN{
    public:
        string name;
        LINK* Link;
        LAN(string nm, LINK* lnk){
            name = nm;
            Link = lnk;
        }
        LINK* link(){
            return Link;
        }
};

#endif