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

        this->nNeighbours = par("nNeighbours").intValue();

        this->neighbours = new User*[this->nNeighbours];

        for(int i=0;i<this->nNeighbours;i++){

            this->neighbours[i] = (User*)getParentModule()->getSubmodule("node", i);
            EV<<this->neighbours[i]->posX<<endl;

        }

        this->neighbours[0]->sendInitialMessage = true;

        marking(this->neighbours[0]);

    }

    void Oracle::finish(){

    }

    void Oracle::marking(User *user){

        //nodes not yet visited but in the graph
        queue<User*> q;
        q.push(user);
        unordered_set<User*> checked;
        unordered_set<User*> unchecked;

        for(int i = 0; i < this->nNeighbours; i++)
            unchecked.insert(this->neighbours[i]);

        while(!q.empty()){
            User* tmp = q.front();
            q.pop();
            checked.insert(tmp);
            unchecked.erase(tmp);

            checkNeighbours(tmp,q,unchecked);

        }

        while(unchecked.size() != 0){
            User* tmp = *(unchecked.begin());
            unchecked.erase(tmp);
            redropUser(tmp);
            //unchecked.clear();
        }
            //checkNewConnections(tmp,checked,unchecked);






        EV<<"CHECKED:"<<endl;
        for(auto itr = checked.begin(); itr != checked.end(); itr++){
            EV<<(*itr)->posX <<" : "<<(*itr)->posY<<endl;
        }
        EV<<"UNCHECKED:"<<endl;

        for(auto itr = unchecked.begin(); itr != unchecked.end(); itr++){
            EV<<(*itr)->posX <<" : "<<(*itr)->posY<<endl;
        }


    }

    void Oracle::checkNeighbours(User* user, queue<User*> &q,unordered_set<User*>& unchecked){

        int myPosX = user->posX;
        int myPosY = user->posY;
        int otherPosX;
        int otherPosY;
        for(auto itr = unchecked.begin(); itr != unchecked.end(); itr++){
            otherPosX = (*itr)->posX;
            otherPosY = (*itr)->posY;

           if(pow(myPosX - otherPosX,2) + pow(myPosY - otherPosY,2) <= pow(user->R,2)){
               q.push(*itr);

           }

        }
    }

    void Oracle::redropUser(User* user){
        user->posX = intuniform(0,600);
        user->posY = intuniform(0,600);
        cDisplayString& dispStr = user->getDisplayString();
        dispStr.setTagArg("p", 0, user->posX);
        dispStr.setTagArg("p", 1, user->posY);


    }

    /*void Oracle::checkNewConnections(User* tmp,unordered_set<User*>& checked,unordered_set<User*>& unchecked){
        for(auto itr)
    }*/
} /* namespace epidemicbroadcast */
