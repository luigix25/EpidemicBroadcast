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



}

} /* namespace epidemicbroadcast */
