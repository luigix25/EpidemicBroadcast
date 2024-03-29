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

void User::initialize(int stage)
{

    if(stage == 0){

        gate("radioIn")->setDeliverOnReceptionStart(true);

        int distributionType = par("distributionType").intValue();
        int RNGPosition = par("RNGPosition").intValue();
        this->noDelay = par("noDelay").boolValue();

        if(distributionType == 0){              //Uniform

            int XLimit = par("XLimit").intValue();
            int YLimit = par("YLimit").intValue();

            this->posX = uniform (0, XLimit, RNGPosition);
            this->posY = uniform (0, YLimit, RNGPosition);

        } else if(distributionType == 1){       //Normal

            double mean = par("mean").doubleValue();
            double stdDev = par("stdDev").doubleValue();

            this->posX = normal (mean, stdDev, RNGPosition);
            this->posY = normal (mean, stdDev, RNGPosition);

        } // distribution type == 2 handled in the oracle


        cDisplayString& dispStr = this->getDisplayString();
        dispStr.setTagArg("p", 0, this->posX);
        dispStr.setTagArg("p", 1, this->posY);


        this->nNeighbours = par("nNeighbours").intValue();


        //EV<<"X: "<<posX<<" Y: "<<posY<<endl;

        this->RNGBackoff        = par("RNGBackoff").intValue();
        this->T                 = par("T").intValue();
        this->m                 = par("m").intValue();
        this->slotSize          = par("slotSize").intValue();

        this->R                 = par("R").intValue();


        this->currentStatus = WAITING;

        return;


    //Redrop occurs between stage 0 and 1 ( if any )
    } else if(stage == 1){

        //Stage 1

        neighbours = new User*[this->nNeighbours];

        for(int i=0;i<this->nNeighbours;i++){

            neighbours[i] = (User*)getParentModule()->getSubmodule("node", i);

        }

        if (this->sendInitialMessage)
        {
            EV<<"Sensing First Message!!"<<endl;
            cMessage *msg = new cMessage("HELO");
            broadcastMessage(msg);
            this->currentStatus = DONE;
            delete msg;
        }

    }

}

void User::handleMessage(cMessage *msg)
{

    // When status is done antenna is switched off
   /* if(this->currentStatus == DONE){
        delete msg;
        return;
    }*/

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
    this->lastMessageTime = currentTime;

    //emit(packetCountSignal,this->receivedPackets);

    simtime_t delayTime;

    switch(this->currentStatus){
        case WAITING:

            scheduledMessage = msg->dup();

            //ScheduleAt wants seconds and slotSize is in milliseconds
            delayTime = this->slotSize * this->T / ONE_SECOND;

            scheduleAt(currentTime + delayTime ,scheduledMessage);               //non posso schedulare nello stesso slot
            this->currentStatus = SCHEDULING;
            EV<<"Status from waiting to scheduling"<<endl;

            //This is necessary in order to compute the convergence time.
            this->firstMessageTime = simTime();


            break;

        case SCHEDULING:
            this->currentStatus = LISTENING;
            EV<<"Status from scheduling to listening"<<endl;

        case LISTENING:
            this->receivedPacketsInTSlots++;
            break;

        /* Waiting for Send and Done */
        default:
            break;

    }


    delete msg;

}

void User::broadcastMessage(cMessage *msg){

    EV<<"Broadcasting"<<endl;
    this->didSend = true;
    for (int i = 0; i < this->nNeighbours; i++)
    {
        //No Self Msg
        if(this->neighbours[i] == this || !this->isInTxRadius(this->neighbours[i]))
            continue;
        cMessage *duplicate = msg->dup();
        sendDirect(duplicate,this->neighbours[i]->gate("radioIn"));
        EV<<"Sending Message"<<endl;
    }


}

void User::handleCollision(){

    //Collision Already Handled or antenna is switched off
    if(this->collided)
        return;

    this->collided = true;
    this->fullCollisions++;

    this->receivedPackets--;



    if(this->currentStatus != WAITING_FOR_SEND && this->currentStatus != DONE){
        this->trickleCollisions++;
    }

    //Message received was not valid

    switch(this->currentStatus){
        case SCHEDULING:                     //time is changed, so there is no collision

            this->receivedPacketsInTSlots = 0;
            cancelAndDelete(this->scheduledMessage);
            EV<<"Status from SCHEDULING to WAITING"<<endl;

            this->currentStatus = WAITING;

            //A collision occured on the first message.
            this->firstMessageTime = -1;

            break;

        case LISTENING:
            this->receivedPacketsInTSlots--;
            break;

        case DONE:
            break;
    }

}

void User::handleSelfMessage(cMessage *msg){


    switch(this->currentStatus){

        case WAITING_FOR_SEND:
            //No matter if i send or not, i do not have to do anything else.
            this->currentStatus = DONE;
            broadcastMessage(msg);
            delete msg;

            break;
        case SCHEDULING:
        case LISTENING:

            if(this->receivedPacketsInTSlots < this->m){

                //-1 altrimenti mando in T slot, ma ascolto in T-1
                simtime_t delayTime = this->slotSize * intuniform(0, this->T-1,this->RNGBackoff) / ONE_SECOND;

                if(this->noDelay)
                    delayTime = 0;

                EV<<"DELAY TIME: ";
                EV<<delayTime<<endl;
                this->currentStatus = WAITING_FOR_SEND;
                scheduleAt(simTime() + delayTime ,msg);

            } else {
                EV<<"Broadcast suppressed"<<endl;
                delete msg;
                this->currentStatus = DONE;

            }


    }



}

void User::finish(){
    //EV<<"Full Collisions: "<<this->fullCollisions<<" "<<"Trickle Collisions: "<<this->trickleCollisions<<" Packets in T Slots:"<<this->receivedPacketsInTSlots<<endl;

    //SimTime recorded just once by the Initiator, for simplicity
    //if (this->sendInitialMessage)
    recordScalar("#FirstMessageTime[slot]", this->firstMessageTime * ONE_SECOND / this->slotSize);

    recordScalar("#PacketCount", this->receivedPackets);
    recordScalar("#TrickleCollision", this->trickleCollisions);
    recordScalar("#FullCollision", this->fullCollisions);

    recordScalar("#ReceivePacketInTSlots", this->receivedPacketsInTSlots);

    if(this->didSend){
        recordScalar("#SendMessage", 1);
    } else{
        recordScalar("#SendMessage", 0);
    }

    if( this->currentStatus == DONE){
        recordScalar("#Covered", 1);
    } else{
        recordScalar("#Covered", 0);
    }

   // EV<<"STATUS: "<<this->currentStatus<<endl;

    int neighborsCount = 0;
    for (int i = 0; i < this->nNeighbours; i++)
    {
        if(this->neighbours[i] == this || !this->isInTxRadius(this->neighbours[i]))
            continue;
        neighborsCount++;
    }

    recordScalar("#Neighbors", neighborsCount);


    //emit(packetCountSignal,this->receivedPackets);

}

bool User::isInTxRadius(User *user){

    return (pow(this->posX - user->posX,2) + pow(this->posY - user->posY,2) <= pow(this->R,2));

}


}; // namespace
