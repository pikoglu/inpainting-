#include <vector>
#include <array>


typedef std::pair<int, int> Point;

struct Label {
    Point point;
    int potential;
    int messageFromLeft;
    int messageFromRight;
    int messageFromTop;
    int messageFromBottom;
    int belief;
};



class Node{
    int index;

    int x;
    int y;

    int leftNeighbor;
    int topNeighbor;
    int rightNeighbor;
    int bottomNeighbor;

    std::vector<Label> nodeConfusionSet; // this isnt entirely equal to the definition of condusionSet in the paper

public:
    Node(int index,int xp,int yp, int Lmax);

    int gety() const {return y;}
    int getx() const {return x;}

    void addLeftNeighbor(int i){leftNeighbor=i;}
    void addRightNeighbor(int i){rightNeighbor=i;}
    void addTopNeighbor(int i){topNeighbor=i;}
    void addBottomNeighbor(int i){bottomNeighbor=i;}

    int getLeftNeighbor(){return leftNeighbor;}
    int getRightNeighbor(){return rightNeighbor;}
    int getTopNeighbor(){return topNeighbor;}
    int getBottomNeighbor(){return bottomNeighbor;}







};



