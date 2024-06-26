#include "node.h"

Node::Node(int i,int xp,int yp,int Lmax) {
    index=i;
    x=xp;
    y=yp;

    leftNeighbor=-1;
    topNeighbor=-1;
    rightNeighbor=-1;
    bottomNeighbor=-1;

    for( int i=0;i<Lmax;i++){
        Label l;
        l.point = Point(0,0);
        l.belief=0;
        l.potential=0;

        l.messageFromTop=0;
        l.messageFromBottom=0;
        l.messageFromLeft=0;
        l.messageFromRight=0;

        nodeConfusionSet.push_back(l);
    }



}


