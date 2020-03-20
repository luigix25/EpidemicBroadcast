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
    }

    int posX = par("posX").intValue();
    int posY = par("posY").intValue();

    EV<<"X: "<<posX<<" Y: "<<posY<<endl;

}

void User::handleMessage(cMessage *msg)
{
    EV << "Received a frame at "<< simTime() << endl;

    if(msg->isSelfMessage()){
        //doQualcosa()

        return;
    }

    simtime_t currentTime = simTime();

    //Collision occured!!!
    if(this->lastMessageTime != null and currentTime == this->lastMessageTime){
        handleCollision();
    }


    this->lastMessageTime = currentTime;


    //this->receivedPackets++;

    // just send back the message received
    if(!this->didTransmit){
        broadcastMessage(msg);
    }

}

void User::broadcastMessage(cMessage *msg){

    for (int i = 0; i < gateSize("gate$o"); i++)
    {
        cMessage *copy = msg->dup();
        send(copy, "gate$o", i);
        EV<<"Sending Message "<<endl;
    }

    delete msg;

    this->transmitted = true;

}

void User::handleCollision(){

    //Collision Already Handled
    if(this->collided)
        return;

    this->collided = true;
    this->collisions++;

    //Message received was not valid
    this->receivedPackets--;

}


}; // namespace
