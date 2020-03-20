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

/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class User : public cSimpleModule
{
    private:
        //Node did transmit or not
        bool didTransmit                = false;

        //Time when last message is received
        simtime_t lastMessageTime;

        //Collision did occur in a time slot
        bool collided                   = false;

        //Sends the message in broadcast
        void broadcastMessage(cMessage *msg);

        void handleCollision();


    protected:

        //Total number of collisions
        unsigned short collisions      = 0;

        //Number of packets received in T time slots
        unsigned short receivedPackets = 0;

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

};

}; // namespace

#endif
