#pragma once
#include <vector>
#include <array>
#include <iostream>
#include "image.h"
#include <limits>
#include <mutex>

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


struct ThreadSafeVector {
    std::vector<std::pair<Label, Point>> data;
    std::mutex mutex;

    void push_back(const std::pair<Label, Point>& item) {
        std::lock_guard<std::mutex> lock(mutex);
        data.push_back(item);
    }

    size_t size() const {
        return data.size();
    }
};


class Node{
    int index;

    Point nodePoint;

    int leftNeighbor;
    int topNeighbor;
    int rightNeighbor;
    int bottomNeighbor;

    std::vector<std::pair<Label,Point> > nodeConfusionSet; // this isnt entirely equal to the definition of condusionSet in the paper

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


    std::vector<std::pair<Label,Point> >& getNodeConfusionSet() { return nodeConfusionSet; }
    const std::vector<std::pair<Label,Point> >& getNodeConfusionSet() const { return nodeConfusionSet; }
    void addLeftNeighbor(int i){leftNeighbor=i;}
    void addRightNeighbor(int i){rightNeighbor=i;}
    void addTopNeighbor(int i){topNeighbor=i;}
    void addBottomNeighbor(int i){bottomNeighbor=i;}

    bool hasLeftNeighbor()const { return leftNeighbor != -1; }
    bool hasRightNeighbor()const { return rightNeighbor != -1; }
    bool hasTopNeighbor() const{ return topNeighbor != -1; }
    bool hasBottomNeighbor() const{ return bottomNeighbor != -1; }


    int getLeftNeighbor()const{return leftNeighbor;}
    int getRightNeighbor()const{return rightNeighbor;}
    int getTopNeighbor()const{return topNeighbor;}
    int getBottomNeighbor()const{return bottomNeighbor;}

    int size()const {return int(nodeConfusionSet.size());}

    int worstBelief();



    bool pushConditioned(const Image &imageInput,const Label& label,int lmin,int lmax,int thresholdSimilarity,int patchSize, Point xq);

    Label label(int i)const;

    bool similarityCondition(const Image &imageInput,const Label labelCandidate,const Point xq,
                             int thresholdSimilarity , int patchSize);

    void pruningThresholdConfusion(int thresholdCondition,int lmin);

    bool inStack(const std::vector<int> &stack);

    void createNodeConfusionSet(const Node &sender, const Image &imageMaskExtended, const Image &imageInput, int patchSize, int thresholdSimilarity, int pruningThresholdConfusion, int lmin, int lmax, std::string path, int s, int w0);
    std::pair<double,Point> messageReceived(const Node  &sender, const Point &coordPatchCandidate, const Image &imageInput, int patchSize, int w0);
    void updateNodeConfusionSet(const Node &sender, const Image &imageMaskExtended, const Image &imageInput, int patchSize, int thresholdSimilarity, int pruningThresholdConfusion, int lmin, int lmax, std::string path, int s, int w0);
    void updateNodeConfusionSetNoPruning(const Node &sender, const Image &imageMaskExtended, const Image &imageInput, int patchSize, int thresholdSimilarity, int thresholdConfusion, int lmin, int lmax, std::string path, int s, int w0);


    void normalizeMessage();

    bool labelPushedThreadSafe(ThreadSafeVector& safeVector, const Image& imageInput, const Label& label, int lmin, int lmax, int thresholdSimilarity, int patchSize, Point xq);

    Label getBestLabel() const{
        Label bestLabel=label(0).copy();
        for (int i=0;i<size();i++){
            if (label(i).belief()>bestLabel.belief()){
                bestLabel=label(i).copy();
                std::cout<<"0 isnt best label"<<std::endl;
            }
        }
        return bestLabel;
    };
};



std::vector<Node> nodesOverMask(Image const &imageMask,int patchsize, int lmax);


Image visualiseNodesAndVertices(Image const &imageMask, std::vector<Node> v,int patchsize) ;


std::vector<Node> assignInitialPriority(const Image& inputImage, const Image& maskExtended, const Image& mask,
                                        int patchSize, int Lmin, int Lmax,
                                        int thresholdConfusion, int thresholdSimilarity, std::string path) ;



std::vector<int> forwardPass(std::vector<Node> &InitialPriority, const Image &imageInput, const Image &imageMaskExtended, Image &orderOfPassage, int patchSize, int thresholdSimilarity, int thresholdConfusion, int lmin, int lmax, std::string path, int w0);


Image imageReconstructed(const std::vector<Node> &InitialPriority, int patchSize,Image inputImage,Image maskImage);

Image visualizeCandidate(const Node &nodeCandidate, const Image &imageInput, int patchSize);

Image backwardPass(std::vector<Node> &InitialPriority, std::vector<int> commitStack, const Image &imageInput, const Image &imageMaskExtended, int patchSize, int thresholdSimilarity, int thresholdConfusion, int lmin, int lmax, std::string path, int w0);

size_t getNodeOfIndex(const std::vector<Node>& InitialPriority, int j);


Image labelRepartition(const std::vector<Node>& priority,int lmax);


Image visualiseMaskOverImage(Image const &imageInput,Image const &imageMask);


void pourcentageNoeudPruned(const std::vector<Node>& priorities,int lmax,int lmin);

Image getConfusionSet(const std::vector<Node>& priorities,const Image& imageInput,int patchSize,int lmax);

void saveNodeCandidate(const Node & nodeCandidate,const Image& imageInput,
                       int patchSize, std::string path,int r,int s,int m);


Image imageReconstructedBlend(const std::vector<Node> &InitialPriority, int patchSize, Image inputImage, Image maskImage, int lmax);
