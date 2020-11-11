//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Oracle.h"

namespace epidemicbroadcast {

    Define_Module(Oracle);

    void Oracle::initialize(){

        this->nNeighbours   = par("nNeighbours").intValue();
        this->XLimit        = par("nNeighbours").intValue();
        this->YLimit        = par("nNeighbours").intValue();
        this->R             = par("R").intValue();
        this->redrop        = par("redrop").boolValue();
        this->RNGPosition   = par("RNGPosition").intValue();

        this->neighbours = new User*[this->nNeighbours];

        for(int i=0;i<this->nNeighbours;i++){
            this->neighbours[i] = (User*)getParentModule()->getSubmodule("node", i);
        }

        this->neighbours[0]->sendInitialMessage = true;

        if(redrop)
            marking();

    }

    void Oracle::finish(){
        //recordScalar("#PacketCount", this->receivedPackets);
    }

    void Oracle::marking(){

        //nodes not yet visited but in the graph
        /*queue<User*> q;
        q.push(user);*/
        unordered_set<User*> checked;
        unordered_set<User*> unchecked;
        queue<User*> q;
        int count = 0;
        int count2 = 0;


        for(int i = 0; i < this->nNeighbours; i++)
            unchecked.insert(this->neighbours[i]);

        while(unchecked.size() != 0){
            q.push((*unchecked.begin()));

            while(!q.empty()){
                User* tmp = q.front();
                q.pop();
                checked.insert(tmp);
                unchecked.erase(tmp);

                checkNeighbours(tmp,q,unchecked);

            }


            if(unchecked.size() != 0){
                count++;
                User* tmp = *(unchecked.begin());
                do{
                    count2++;
                    redropUser(tmp);
                }while(!checkNewConnections(tmp,checked));


            }

            /*while(unchecked.size() != 0){
                User* tmp = *(unchecked.begin());
                unchecked.erase(tmp);
                redropUser(tmp);
            }*/
            //checkNewConnections(tmp,checked,unchecked);
        }


        EV<<"CHECKED:"<<checked.size() << endl;
        /*for(auto itr = checked.begin(); itr != checked.end(); itr++){
            EV<<(*itr)->posX <<" : "<<(*itr)->posY<<endl;
        }*/
        EV<<"UNCHECKED:"<<unchecked.size()<<endl;

        for(auto itr = unchecked.begin(); itr != unchecked.end(); itr++){
            EV<<(*itr)->posX <<" : "<<(*itr)->posY<<endl;
        }

        EV << "Numero Redrop: " << count << endl;
        EV << "Numero Redrop totali: " << count2 << endl;

    }

    void Oracle::checkNeighbours(User* user, queue<User*> &q,unordered_set<User*>& unchecked){

        int myPosX = user->posX;
        int myPosY = user->posY;
        int otherPosX;
        int otherPosY;
        for(auto itr = unchecked.begin(); itr != unchecked.end(); itr++){
            otherPosX = (*itr)->posX;
            otherPosY = (*itr)->posY;

           if(pow(myPosX - otherPosX,2) + pow(myPosY - otherPosY,2) <= pow(this->R,2)){
               q.push(*itr);

           }

        }
    }

    void Oracle::redropUser(User* user){
        user->posX = intuniform(0,this->XLimit,this->RNGPosition);
        user->posY = intuniform(0,this->YLimit,this->RNGPosition);
        cDisplayString& dispStr = user->getDisplayString();
        dispStr.setTagArg("p", 0, user->posX);
        dispStr.setTagArg("p", 1, user->posY);


    }

    bool Oracle::checkNewConnections(User* tmp,unordered_set<User*>& checked){
        for(auto itr = checked.begin(); itr != checked.end(); itr++){
            if(isInTxRadius(tmp,*itr))
                return true;
        }
        return false;
    }

    bool Oracle::isInTxRadius(User *a, User *b){

        //Radius size in unique
        return (pow(a->posX - b->posX,2) + pow(a->posY - b->posY,2) <= pow(this->R,2));

    }
} /* namespace epidemicbroadcast */
