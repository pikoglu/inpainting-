#pragma once
#include <vector>
#include <array>
#include <iostream>
#include "image.h"
#include <limits>

typedef std::pair<int, int> Point;

class Label {
    Point _point;
    int potential;
    double messageFromLeft;
    double messageFromRight;
    double messageFromTop;
    double messageFromBottom;

public:

    Label(Point p, int pot, int mLeft = 0, int mRight = 0, int mTop = 0, int mBottom = 0)
            : _point(p), potential(pot), messageFromLeft(mLeft), messageFromRight(mRight),
              messageFromTop(mTop), messageFromBottom(mBottom) {}

    double belief()const{return -potential-messageFromLeft- messageFromRight-messageFromTop-messageFromBottom;}

    const Point & point()const {return _point ;}

    double getPotential(){return potential;}

    void setMessageFromLeft(double message){messageFromLeft=message;}
    void setMessageFromRight(double  message){messageFromRight=message;}
    void setMessageFromTop(double message){messageFromTop=message;}
    void setMessageFromBottom(double message){messageFromBottom=message;}

    double getMessageFromLeft() const { return messageFromLeft; }
    double getMessageFromRight() const { return messageFromRight; }
    double getMessageFromTop() const { return messageFromTop; }
    double getMessageFromBottom() const { return messageFromBottom; }

    int getx()const{return _point.first;}
    int gety()const {return _point.second;}

    int minMessage()const{return std::min(std::min(messageFromLeft>0?messageFromLeft:0,messageFromRight>0?messageFromRight:0),std::min(messageFromTop>0?messageFromTop:0,messageFromBottom>0?messageFromBottom:0));}

    int maxMessage()const {return std::max(std::max(messageFromLeft,messageFromRight),std::max(messageFromTop,messageFromBottom));}

    void normalizeMessage(){int minMessage_=minMessage();
        messageFromLeft-=minMessage_;
        messageFromRight-=minMessage_;
        messageFromTop-=minMessage_;
        messageFromBottom-=minMessage_;
    }

    Label copy() const {
        return Label(_point, potential, messageFromLeft, messageFromRight, messageFromTop, messageFromBottom);
    }



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

    Node(const Node& other)
            : index(other.index)
            , nodePoint(other.nodePoint)
            , leftNeighbor(other.leftNeighbor)
            , topNeighbor(other.topNeighbor)
            , rightNeighbor(other.rightNeighbor)
            , bottomNeighbor(other.bottomNeighbor)
            , nodeConfusionSet(other.nodeConfusionSet) // This creates a shallow copy of the vector
        {}

    int gety() const {return nodePoint.second;}
    int getx() const {return nodePoint.first;}
    Point point()const{return nodePoint;}
    int getIndex()const {return index;}


    std::vector<Label>& getNodeConfusionSet() { return nodeConfusionSet; }
    const std::vector<Label>& getNodeConfusionSet() const { return nodeConfusionSet; }
    void addLeftNeighbor(int i){leftNeighbor=i;}
    void addRightNeighbor(int i){rightNeighbor=i;}
    void addTopNeighbor(int i){topNeighbor=i;}
    void addBottomNeighbor(int i){bottomNeighbor=i;}

    bool hasLeftNeighbor() { return leftNeighbor != -1; }
    bool hasRightNeighbor() { return rightNeighbor != -1; }
    bool hasTopNeighbor() { return topNeighbor != -1; }
    bool hasBottomNeighbor() { return bottomNeighbor != -1; }


    int getLeftNeighbor(){return leftNeighbor;}
    int getRightNeighbor(){return rightNeighbor;}
    int getTopNeighbor(){return topNeighbor;}
    int getBottomNeighbor(){return bottomNeighbor;}

    int size()const {return int(nodeConfusionSet.size());}

    int worstBelief();



    void pushConditioned(const Image &imageInput,const Label& label,int lmin,int lmax,int thresholdSimilarity,int patchSize);

    Label label(int i)const;

    bool similarityCondition(const Image &imageInput,const Point &pointLabel,int thresholdSimilarity , int patchSize);
    void pruningThresholdConfusion(int thresholdCondition,int lmin);

    bool inStack(const std::vector<int> &stack);

    void createNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int pruningThresholdConfusion,int lmin,int lmax);
    double messageReceived(  const Node  &sender, const Point &coordPatchCandidate, const Image &imageInput,int patchSize);
    void updateNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int pruningThresholdConfusion,int lmin,int lmax);

    void normalizeMessage();

    Label getBestLabel() const{
        Label bestLabel=label(0).copy();
        for (int i=0;i<size();i++){
            if (label(i).belief()>bestLabel.belief()){
                bestLabel=label(i).copy();
                std::cout<<"0 isnt best label"<<std::cout;
            }
        }
        return bestLabel;
    };
};



std::vector<Node> nodesOverMask(Image const &imageMask,int patchsize, int lmax);


Image visualiseNodesAndVertices(Image const &imageMask, std::vector<Node> v,int patchsize) ;


std::vector<Node> assignInitialPriority( const Image& inputImage,const Image& maskExtended,const Image& mask,
                                                       int patchSize, int Lmin, int Lmax,
                                                       int thresholdConfusion, int thresholdSimilarity) ;



std::vector<int> forwardPass(std::vector<Node> &InitialPriority,const Image &imageInput, const Image &imageMaskExtended,Image &orderOfPassage,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax);


Image imageReconstructed(const std::vector<Node> &InitialPriority, int patchSize,Image inputImage,Image maskImage);

Image visualizeCandidate(const std::vector<Node> &InitialPriority,const Image &imageInput,int patchSize, int index);

Image backwardPass(std::vector<Node> &InitialPriority,std::vector<int> commitStack,const Image &imageInput, const Image &imageMaskExtended,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax);

size_t getNodeOfIndex(const std::vector<Node>& InitialPriority, int j);
