#include <vector>
#include <array>


typedef std::array<int, 5> Messages;
typedef std::pair<int, int> Point;
typedef std::pair<Point, Messages> Belief;
typedef std::vector<Belief> ConfusionSet;

class Node{
    int index;
    int x;
    int y;
    int leftNeighbor;
    int topNeighbor;
    int rightNeighbor;
    int bottomNeighbor;
    std::vector<int> messages[4];
public:
    Node(int index,int xp,int yp);

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

void forwardpass(std::vector<ConfusionSet> &confusionSets,std::vector<Node> const &Nodes );
