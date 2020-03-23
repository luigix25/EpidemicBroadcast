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

    int posX = par("posX").intValue();
    int posY = par("posY").intValue();

    EV<<"X: "<<posX<<" Y: "<<posY<<endl;

    this->RNGBackoff        = par("RNGBackoff").intValue();
    this->maxBackoffWait    = par("maxBackoffWait").intValue();
    this->slotSize          = par("slotSize").intValue();

    this->currentStatus = WAITING;

    if (par("sendInitialMessage").boolValue())
    {
        EV<<"MASTER!!"<<endl;
        cMessage *msg = new cMessage("tictocMsg");
        broadcastMessage(msg);
        this->currentStatus = DONE;
        delete msg;
    }

}

void User::handleMessage(cMessage *msg)
{
    EV << "Received a frame at "<< simTime() << endl;

    if(msg->isSelfMessage()){
        handleSelfMessage(msg);
        return;
    }

    simtime_t currentTime = simTime();

    //Collision occurred!!!
    if(currentTime == this->lastMessageTime){
        EV<<"Collision!! "<<currentTime<<endl;
        handleCollision();
        delete msg;
        return;
    }

    //No collision, slot is changed since last time!
    this->collided = false;
    this->receivedPackets++;

    simtime_t delayTime;

    switch(this->currentStatus){
        case WAITING:

            scheduledMessage = msg->dup();
            //TODO: sistemareProb, /1000 brutto
            delayTime = this->slotSize * intuniform(1, this->maxBackoffWait,this->RNGBackoff) / 1000.0;

            scheduleAt(currentTime + delayTime ,scheduledMessage);               //non posso schedulare nello stesso slot
            this->currentStatus = SCHEDULING;
            EV<<"Status from waiting to scheduling"<<endl;

            break;

        case SCHEDULING:
            this->currentStatus = LISTENING;
            EV<<"Status from scheduling to listening"<<endl;

        case LISTENING:
            this->receivedPacketsInTSlots++;
            break;

        case DONE:
            break;

    }

    this->lastMessageTime = currentTime;

    delete msg;

}

void User::broadcastMessage(cMessage *msg){

    for (int i = 0; i < gateSize("gate$o"); i++)
    {
        cMessage *duplicate = msg->dup();
        //TODO: diviso 1000 è brutto
        sendDelayed(duplicate,this->slotSize/1000.0, "gate$o", i);
        EV<<"Sending Message"<<endl;
    }

}

void User::handleCollision(){

    //Collision Already Handled
    if(this->collided)
        return;

    this->collided = true;
    this->collisions++;
    this->receivedPackets--;

    //Message received was not valid

    switch(this->currentStatus){
        case SCHEDULING:                     //time is changed, so there is no collision

            this->receivedPacketsInTSlots = 0;
            cancelAndDelete(this->scheduledMessage);
            EV<<"Status from SCHEDULING to WAITING"<<endl;

            this->currentStatus = WAITING;

            break;

        case LISTENING:
            this->receivedPacketsInTSlots--;
            break;

        case DONE:
            break;
    }

}

void User::handleSelfMessage(cMessage *msg){

    //No matter if i send or not, i do not have to do anything else.
    this->currentStatus = DONE;

    int m = par("m").intValue();

    if(this->receivedPacketsInTSlots < m){
        broadcastMessage(msg);
    } else {
        EV<<"Broadcast suppressed"<<endl;
    }

    delete msg;

}

void User::finish(){
    EV<<"Collisions: "<<this->collisions<<" "<<"Packets: "<<this->receivedPackets<<" Packets in T Slots:"<<this->receivedPacketsInTSlots<<endl;

}



}; // namespace
