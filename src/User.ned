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

package epidemicbroadcast;

//
// Immediately sends out any message it receives. It can optionally generate
// a message at the beginning of the simulation, to bootstrap the process.
//

simple User
{
    parameters:
        bool sendInitialMessage 			= default(false);
        
        int m								= default(3);
        int T								= default(5);
        
		int RNGPosition						= default(1);
    	int RNGBackoff						= default(2);
    	
    	int XLimit							= default(600);
    	int YLimit							= default(600);
            	
		int slotSize @unit(milliseconds)	= default(1000ms);
				
		int posX = default(intuniform (0, XLimit, RNGPosition));
		int posY = default(intuniform (0, YLimit, RNGPosition));    		
		@display("p=$posX,$posY");    
		
		@signal[packets](source="packets";type=int);
		@statistic[packets](title="packets";record=mean,max,count);		
		    
    gates:
        inout 	gate[];
}

