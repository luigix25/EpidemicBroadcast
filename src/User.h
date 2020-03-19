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
  protected:

    bool transmitted = false;


    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void broadcastMessage(cMessage *msg);

};

}; // namespace

#endif
