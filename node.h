#pragma once
#include <vector>
#include <array>
#include <iostream>
#include "image.h"
#include <cassert>
#include <limits>

typedef std::pair<int, int> Point;

class Label {
    Point _point;
    int potential;
    int messageFromLeft;
    int messageFromRight;
    int messageFromTop;
    int messageFromBottom;

public:

    Label(Point p, int pot, int mLeft = 0, int mRight = 0, int mTop = 0, int mBottom = 0)
            : _point(p), potential(pot), messageFromLeft(mLeft), messageFromRight(mRight),
              messageFromTop(mTop), messageFromBottom(mBottom) {}

    int belief()const{return -potential-messageFromLeft- messageFromRight-messageFromTop-messageFromBottom;}

    Point point(){return _point ;}
};



class Node{
    int index;

    Point nodePoint;

    int leftNeighbor;
    int topNeighbor;
    int rightNeighbor;
    int bottomNeighbor;

    std::vector<Label> nodeConfusionSet; // this isnt entirely equal to the definition of condusionSet in the paper

public:
    Node(int index,Point nodePoint,int lmax);

    int gety() const {return nodePoint.second;}
    int getx() const {return nodePoint.first;}
    Point point(){return nodePoint;}

    void addLeftNeighbor(int i){leftNeighbor=i;}
    void addRightNeighbor(int i){rightNeighbor=i;}
    void addTopNeighbor(int i){topNeighbor=i;}
    void addBottomNeighbor(int i){bottomNeighbor=i;}

    int getLeftNeighbor(){return leftNeighbor;}
    int getRightNeighbor(){return rightNeighbor;}
    int getTopNeighbor(){return topNeighbor;}
    int getBottomNeighbor(){return bottomNeighbor;}

    int size(){return int(nodeConfusionSet.size());}

    int worstBelief();

    void pushConditioned(const Label& label,int lmin,int lmax);

    Label label(int i);

    bool similarityCondition(const Image &imageInput, Point &pointLabel,int thresholdSimilarity , int patchSize);
    bool thresholdConfusion(int thresholdCondition,int lmin);
};



std::vector<Node> nodesOverMask(Image const &imageMask,int patchsize, int lmax);


Image visualiseNodesAndVertices(Image const &imageMask, std::vector<Node> v,int patchsize) ;


std::vector<Node> assignInitialPriority( const Image& inputImage,const Image& maskExtended,const Image& mask,
                                                       int patchSize, int Lmin, int Lmax,
                                                       int thresholdConfusion, int thresholdSimilarity) ;



