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

#include <omnetpp.h>
#include "User.h"
#include <unordered_set>
#include <queue>
using namespace omnetpp;
using namespace std;

#ifndef ORACLE_H_
#define ORACLE_H_


namespace epidemicbroadcast {

class Oracle : public cSimpleModule{

    private:
        User** neighbours;
        int nNeighbours;

        int R;
        int XLimit, YLimit;
        bool redrop;

        int RNGPosition;

        // For stats
        int totalNumberOfRedrops;
        int unlinkedNodes;              /* Before Redrop if any */


        void checkNeighbours(User*, queue<User*> &,unordered_set<User*>&);
        void marking();
        void redropUser(User*);
        bool isInTxRadius(User *, User *);
        bool checkNewConnections(User*,unordered_set<User*>&);


    public:
        virtual void initialize();
        //virtual void handleMessage(cMessage *msg);
        virtual void finish();

};

} /* namespace epidemicbroadcast */

#endif /* ORACLE_H_ */
