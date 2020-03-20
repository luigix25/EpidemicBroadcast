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

#include "User.h"

namespace epidemicbroadcast {

Define_Module(User);

void User::initialize()
{
    if (par("sendInitialMessage").boolValue())
    {
        EV<<"MASTER!!"<<endl;
        cMessage *msg = new cMessage("tictocMsg");
        broadcastMessage(msg);
        this->messageReceived = true;
    }

    int posX = par("posX").intValue();
    int posY = par("posY").intValue();

    this->currentStatus = WAITING;

    EV<<"X: "<<posX<<" Y: "<<posY<<endl;

}

void User::handleMessage(cMessage *msg)
{
    EV << "Received a frame at "<< simTime() << endl;

    if(msg->isSelfMessage()){
        handleSelfMessage(msg);
        return;
    }

    simtime_t currentTime = simTime();

    //Collision occured!!!
    if(currentTime == this->lastMessageTime){
        EV<<"Collisione!! "<<currentTime<<endl;
        handleCollision();
        return;
    }

    //No collision, slot is changed since last time!
    this->collided = false;


    switch(this->currentStatus){
        case WAITING:

            scheduledMessage = msg->dup();
            //TODO: sistemareProb
            scheduleAt(currentTime+intuniform(0,2),scheduledMessage);
            this->currentStatus = SCHEDULING;
            EV<<"Status from waiting to scheduling"<<endl;

            break;

        case SCHEDULING:
            this->currentStatus = LISTENING;
            EV<<"Status from waiting to scheduling"<<endl;


        case LISTENING:
            this->receivedPackets++;
            break;

    }

    this->lastMessageTime = currentTime;


}

void User::broadcastMessage(cMessage *msg){

    for (int i = 0; i < gateSize("gate$o"); i++)
    {
        cMessage *copy = msg->dup();
        send(copy, "gate$o", i);
        EV<<"Sending Message"<<endl;
    }

    delete msg;

}

void User::handleCollision(){

    //Collision Already Handled
    if(this->collided)
        return;

    this->collided = true;
    this->collisions++;

    //Message received was not valid

    switch(this->currentStatus){
        case SCHEDULING:                     //time is changed, so there is no collision

            this->receivedPackets = 0;
            cancelEvent(this->scheduledMessage);
            delete this->scheduledMessage; //(?)
            EV<<"Status from SCHEDULING to WAITING"<<endl;

            this->currentStatus = WAITING;

            break;

        case LISTENING:
            this->receivedPackets--;
            break;
    }

}

void User::handleSelfMessage(cMessage *msg){

    int m = par("m").intValue();

    if(this->receivedPackets < m){
        broadcastMessage(msg);
        //TODO: controllare gestione memoria delete msg;
    } else {
        EV<<"Broadcast suppressed"<<endl;
    }

}

void User::finish(){
    EV<<"Collisions: "<<this->collisions<<" "<<"Packets: "<<this->receivedPackets<<endl;

}




}; // namespace
