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
        this->XLimit        = par("XLimit").intValue();
        this->YLimit        = par("YLimit").intValue();
        this->R             = par("R").intValue();
        this->redrop        = par("redrop").boolValue();
        this->RNGPosition   = par("RNGPosition").intValue();

        this->neighbours = new User*[this->nNeighbours];
        this->totalNumberOfRedrops = 0;

        for(int i=0;i<this->nNeighbours;i++){
            this->neighbours[i] = (User*)getParentModule()->getSubmodule("node", i);
        }

        this->neighbours[0]->sendInitialMessage = true;

        //if(redrop)
        marking();

    }

    void Oracle::finish(){
        recordScalar("#unlinkedNodes", this->unlinkedNodes);
        //recordScalar("#totalNumberOfRedrops", this->totalNumberOfRedrops);

        EV<<"Unlinked: "<<this->unlinkedNodes<<endl;
        EV<<"totalNumberOfRedrops: "<<this->totalNumberOfRedrops<<endl;

    }

    void Oracle::marking(){

        //nodes not yet visited but in the graph
        /*queue<User*> q;
        q.push(user);*/
        unordered_set<User*> checked;
        unordered_set<User*> unchecked;
        queue<User*> q;

        bool firstIteration = true;

        /* All the nodes are uncheked */
        for(int i = 0; i < this->nNeighbours; i++)
            unchecked.insert(this->neighbours[i]);

        while(unchecked.size() != 0){
            /* First element of the unchecked */
            q.push((*unchecked.begin()));


            /* In the this while starting from q [that initially is the first element] i create a queue with all the connected nodes */
            while(!q.empty()){
                User* tmp = q.front();
                q.pop();                    /* pop is a void function*/
                checked.insert(tmp);
                unchecked.erase(tmp);

                checkNeighbours(tmp,q,unchecked);       /* all the unchecked neighbours of tmp are added to q */

            }

            /* unlikedNodes is set to the initial number of unliked nodes and not updated afterwards*/
            if(firstIteration){
                this->unlinkedNodes = unchecked.size();
                firstIteration = false;
            }

            if(!this->redrop){
                return;
            }

            /* Redrop of the unlinked nodes from the "main" cluster ONE at a time*/
            if(unchecked.size() != 0){
                User* tmp = *(unchecked.begin());
                do{
                    this->totalNumberOfRedrops++;
                    redropUser(tmp);
                }while(!checkNewConnections(tmp,checked)); /* Check that is connected to AT LEAST one connected node */


            }

        }



    }

    void Oracle::checkNeighbours(User* user, queue<User*> &q,unordered_set<User*>& unchecked){

        double myPosX = user->posX;
        double myPosY = user->posY;
        double otherPosX;
        double otherPosY;
        for(auto itr = unchecked.begin(); itr != unchecked.end(); itr++){

            if(isInTxRadius(user,*itr))
                    q.push(*itr);

        }
    }

    void Oracle::redropUser(User* user){
       // user->posX = intuniform(0,this->XLimit,this->RNGPosition);
       // user->posY = intuniform(0,this->YLimit,this->RNGPosition);
        user->posX = uniform(0,this->XLimit,this->RNGPosition);
        user->posY = uniform(0,this->YLimit,this->RNGPosition);
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
