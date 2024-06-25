#include "node.h"

Node::Node(int i,int xp,int yp) {
    index=i;
    x=xp;
    y=yp;

    leftNeighbor=-1;
    topNeighbor=-1;
    rightNeighbor=-1;
    bottomNeighbor=-1;
}


void forwardpass(std::vector<ConfusionSet> &confusionSets,std::vector<Node> const &Nodes ){

}
