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


unsigned int TIME=0;
vector <string> traces;


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

////////////////////LINK/////////////////////////////


class LINK{
    public:
        int total_msg;
        string LAN;
        vector <int> Data;
        LINK(string lan_name){
            total_msg = 0;
            LAN = lan_name;
        }
        void load(vector <int> msg);
        void clear();
        vector <int> accept();
};


void LINK::load(vector <int> msg){
    Data.insert(Data.end(), msg.begin(), msg.end());
    total_msg += msg.size()/3;
}

void LINK::clear(){
    Data.clear();
    total_msg = 0;
}

vector <int> LINK::accept(){
    vector <int> data;
    data = Data;
    return data;
}

//////////////////////PORT//////////////////////////////


class PORT{
    public:
        string status;
        int ID;
        LINK* Link;
        string port_lan;

        PORT(int id, string lan_name){
            ID = id+1;
            status = "DP";
            Link = NULL;
            port_lan=lan_name;
        }
        void linking(LINK &link){
            Link = &link;
            }
};


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
            RP = 0;
            ID = id;
            Name = "B" + to_string(id);
            rootDis = 0;
            root_ID = ID;
            sending_brg = ID;
        }

        void write(vector <int> message, int port);
        vector <int> read(int port);
        void forward();
        void generate();
        void check_update(vector <int> msg, int msg_port); 
        void add_port(int port, string lan_name);
        void status_update(string update, int port, string previous);
};

void BRDG::status_update(string update, int port, string previous){
    if (update == "NP"){
        ports[port-1].status = update;
        return;
    }
    if (RP>0) ports[RP-1].status = previous;
    RP = port;
    ports[port-1].status = update;
}

void BRDG::add_port(int port, string lan_name){
    ports.push_back(PORT(port, lan_name));
    NonNull.push_back(port+1);
}


void BRDG::check_update(vector <int> msg, int msg_port){
    for(int i = 0; i < int(msg.size()/3); i++){
        if (msg[3*i+2] != ID && msg[3*i+2] > 0 && find(NonNull.begin(), NonNull.end(), msg_port) != NonNull.end()){
            traces.push_back(to_string(TIME) + " r " + Name + " (B" + to_string(msg[3*i]) + ", " + to_string(msg[3*i+1])+", B" + to_string(msg[3*i+2]) + ")    port: " + to_string(msg_port));
            //cout<<to_string(TIME) +" r "<<Name<<" "<< "(B" + to_string(msg[3*i]) + ", " + to_string(msg[3*i+1])+", B" + to_string(msg[3*i+2]) + ")    port: "<<to_string(msg_port)<<endl;
            if (msg[3*i] < root_ID){
                root_ID = msg[3*i];
                rootDis = msg[3*i+1] + 1;
                sending_brg = msg[3*i+2];
                status_update("RP", msg_port, "DP");
            }
            else if (msg[3*i] == root_ID  && msg[3*i+1] + 1 < rootDis){
                rootDis = msg[3*i+1] + 1;
                if (sending_brg < ID){
                    NonNull.erase(find(NonNull.begin(), NonNull.end(), RP));
                    status_update("RP", msg_port, "NP");
                }
                else{
                    status_update("RP", msg_port, "DP");
                }
                sending_brg = msg[3*i+2];
            }
            else if (msg[3*i] == root_ID && msg[3*i+1] + 1 >= rootDis && msg[3*i+2] < sending_brg){
                if (sending_brg < ID){
                    NonNull.erase(find(NonNull.begin(), NonNull.end(), RP));
                    status_update("RP", msg_port, "NP");
                }
                sending_brg = msg[3*i+2];
            }
            else if (msg[3*i] == root_ID && msg[3*i+1] + 1 >= rootDis && msg[3*i+2] > sending_brg && msg_port!= RP){
                if (sending_brg < ID){
                    status_update("NP", msg_port, "NoChange");
                    NonNull.erase(find(NonNull.begin(), NonNull.end(), msg_port));
                }          
            }
            else if (msg[3*i] == root_ID && msg[3*i+1] +1 >= rootDis && msg[3*i+2] == sending_brg && msg_port != RP){
                status_update("NP", msg_port, "NoChange");
                NonNull.erase(find(NonNull.begin(), NonNull.end(), msg_port));
            }
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
        if (ports[i].status != "NP"){
            ports[i].Link->load(msg);
            traces.push_back(to_string(TIME) + " s " + Name + " (" + Name + ", 0, " + Name + ")    port: " + to_string(i+1));
            //cout<<to_string(TIME) +" s "<<Name<<" "<< "(" + Name + ", 0, " + Name + ")    port: "<<to_string(i+1)<<endl;
        }   
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
    for (int i=0; i<BUFFER.size(); i++){                   //update with gathered information
        check_update(BUFFER[i], NonNull[i]);
    }
    for (int i = 0; i<BUFFER.size(); i++){
        for (int j=0; j< BUFFER[i].size()/3; j++){
            if (BUFFER[i][3*j] > 0){
                BUFFER[i][3*j+1]++;
                BUFFER[i][3*j+2] = ID;
            }
        }
    }
    for (int i=0; i < NonNull.size(); i++){
        ports[NonNull[i]-1].Link->load(filter(slicing(BUFFER, i)));
    }
}

#endif