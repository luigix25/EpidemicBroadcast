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

#ifndef __EPIDEMICBROADCAST_TCX_H
#define __EPIDEMICBROADCAST_TCX_H

#include <omnetpp.h>

using namespace omnetpp;

namespace epidemicbroadcast {

enum status{WAITING, SCHEDULING, LISTENING};

/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class User : public cSimpleModule
{
    private:


        //Time when last message is received
        simtime_t lastMessageTime = -1;

        //Current Status of the Node
        status currentStatus;

        //Collision did occur in a time slot
        bool collided                   = false;

        bool messageReceived            = false;

        cMessage* scheduledMessage;

        //Sends the message in broadcast
        void broadcastMessage(cMessage *msg);

        void handleCollision();
        void handleSelfMessage(cMessage *msg);

    protected:

        //Total number of collisions
        unsigned short collisions      = 0;

        //Number of packets received in T time slots
        unsigned short receivedPacketsInTSlots  = 0;
        //Number of packets received globally without collisions
        unsigned short receivedPackets          = 0;

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

}; // namespace

#endif
