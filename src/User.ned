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
        //bool sendInitialMessage 			= default(false);
        
        int m 								= default(1);
        int T 								= default(1);
        int R;
                
		int RNGPosition						= default(1);
    	int RNGBackoff						= default(2);
    	
    	int XLimit							= default(2000);					
    	int YLimit							= default(2000);
            	
    	int nNeighbours;
            	
		int slotSize @unit(milliseconds)	= default(1000ms);
		
		int distributionType				= default(0);			//0 for uniform 1 for normal
		double mean							= default(0);			//mean for normal
		double stdDev						= default(1);			//std dev
		
		bool noDelay						= default(false);	
						
		@display("p=0,0;r=$R");    

				    
    gates:
    	input radioIn @directIn;
    
}

