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

package epidemicbroadcast.simulations;

import epidemicbroadcast.User;

//
// Two instances (tic and toc) of Txc connected.
//

network EpidemicBroadcast
{

	parameters:
		int nUser;
		node[0].sendInitialMessage = true;
		int range = default(300);
		
    submodules:
    	node[nUser]: User;
           
    connections:
        for i=0..sizeof(node)-2, for j=(i+1)..sizeof(node)-1 {    		
			node[i].gate++ <--> {delay = 1000ms;} <--> node[j].gate++ if pow(node[i].posX-node[j].posX, 2)+pow(node[i].posY-node[j].posY, 2) < range*range;
    	}   	
    	

}
