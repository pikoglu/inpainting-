#include "node.h"




Node::Node(int i,Point p,int lmax) {
    index=i;


    nodePoint=Point(p);

    leftNeighbor=-1;
    topNeighbor=-1;
    rightNeighbor=-1;
    bottomNeighbor=-1;

    nodeConfusionSet.reserve(size_t(lmax));
}
std::vector<Node> nodesOverMask(Image const &imageMask,int patchsize, int lmax) {


    //Ne fonctionne que dans le cas impaire
    std::vector<Node> v;
    int index=0;




    for (int y=patchsize/2+1;y<imageMask.height()-patchsize/2;y+=patchsize/2){
        for (int x=patchsize/2+1;x<imageMask.width()-patchsize/2;x+=patchsize/2){//attention aux nombres paires



             if (imageMask(x,y)>128){
                Point p(x,y);

                Node n(index,p,lmax);
                v.push_back(n);

                //is there an already existing neighbors at the left ???

                if (1<v.size()){
                    if (v[v.size()-2].getx()==x-patchsize/2 && v[v.size()-2].gety()==y){
                        v.back().addLeftNeighbor(int(v.size()-2));
                        v[v.size()-2].addRightNeighbor(int(v.size()-1));


                    }
                }
                //is there an already existing neighbors at the top ???

                for (size_t i=0; i<v.size();i++){
                    if (v[i].getx()==x && v[i].gety()==y-patchsize/2){
                        v.back().addTopNeighbor(int(i));
                        v[i].addBottomNeighbor(int(v.size()-1));
                        break;

                    }
                }
             }
         }
    }
    return  v;
}

void Node::pushConditioned(const Label& label,int lmin,int lmax) {
    if (nodeConfusionSet.size()<lmin){nodeConfusionSet.push_back(label);}
    else{
        if (label.belief()>nodeConfusionSet.back().belief()){
            if(int(nodeConfusionSet.size())==lmax){
                nodeConfusionSet.pop_back();
            }
            //this can happen only if previous is done
            if (label.belief()<=nodeConfusionSet.back().belief()){
                nodeConfusionSet.push_back(label);
            }
            else{

                //can be improved by inverse browsing
                for (size_t i=0;i<nodeConfusionSet.size();i++){
                    if (nodeConfusionSet[i].belief()<label.belief()){
                        nodeConfusionSet.insert(nodeConfusionSet.begin()+i,label);
                        break;
                    }
                }
            }
        }
    }
}

Label Node::label(int i){
    assert(i<nodeConfusionSet.size());
    return nodeConfusionSet[i];}

bool Node::similarityCondition(const Image &imageInput, Point &pointLabel,int thresholdSimilarity , int patchSize){
    for (size_t i=0;i<size();i++){
        if (imageInput.ssd(pointLabel,label(i).point(),patchSize)<thresholdSimilarity){
            return false;
        }

    }
    return true;
}



bool Node::thresholdConfusion(int thresholdConfusion,int lmin){

    if (nodeConfusionSet.size() > lmin) {
        bool breaked=false;
        int maxBelief = -nodeConfusionSet[0].belief();

        size_t validIndex = 0;
        for (size_t i = 0; i < nodeConfusionSet.size(); ++i) {
            if (nodeConfusionSet[i].belief() - maxBelief < thresholdConfusion) {
                validIndex = i;
                breaked=true;
                break;
            }
        }

        if (breaked){

            if (validIndex < lmin) {
                validIndex = lmin;
            }
            while (nodeConfusionSet.size() > validIndex) {
                   nodeConfusionSet.pop_back();
            }
        }
    }
}

int Node::worstBelief(){
    if (nodeConfusionSet.empty()){return -std::numeric_limits<int>::max();}
    return nodeConfusionSet.back().belief() ;}

Image visualiseNodesAndVertices(Image const &imageMask, std::vector<Node> v,int patchsize) {
    Image nodesAndVertices(imageMask.clone());

    for (size_t i=0; i<v.size();i++){
        Node n=v[i];
        //std::cout<<n.getx()<<" "<<n.gety()<<std::endl;
        nodesAndVertices(n.getx(),n.gety(),0)=255;
        nodesAndVertices(n.getx(),n.gety(),1)=0;
        nodesAndVertices(n.getx(),n.gety(),2)=0;



        if ( n.getTopNeighbor()!=-1){
            for (int j=1;j<patchsize/2;j++){
                nodesAndVertices(n.getx(),n.gety()-j,1)=255;
                nodesAndVertices(n.getx(),n.gety()-j,0)=0;
                nodesAndVertices(n.getx(),n.gety()-j,2)=0;
            }
        }



        // Visualize bottom neighbor vertices as green
        if (n.getBottomNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                nodesAndVertices(n.getx() , n.gety()+j, 0) = 0;
                nodesAndVertices(n.getx() , n.gety()+j, 1) = 255;
                nodesAndVertices(n.getx() , n.gety()+j, 2) = 0;
            }
        }

        // Visualize left neighbor vertices as green
        if (n.getLeftNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                nodesAndVertices(n.getx()- j, n.gety() , 0) = 0;
                nodesAndVertices(n.getx()- j, n.gety() , 1) = 255;
                nodesAndVertices(n.getx()- j, n.gety() , 2) = 0;
            }
        }
        // Visualize right neighbor vertices as green
        if (n.getRightNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                nodesAndVertices(n.getx()+ j, n.gety() , 0) = 0;
                nodesAndVertices(n.getx()+ j, n.gety() , 1) = 255;
                nodesAndVertices(n.getx()+ j, n.gety(), 2) = 0;
            }
        }
    }
    return nodesAndVertices;
}


std::vector<Node> assignInitialPriority( const Image& imageInput,const Image& imageMaskExtended,const Image& imageMask,
                                                       int patchSize, int lmin, int lmax,
                                                       int thresholdConfusion, int thresholdSimilarity) {


    std::vector<Node> confusionSets=nodesOverMask(imageMask,patchSize,lmax);


    int progression=0;

    // Iterate over each node
    for (size_t i=0;i<confusionSets.size();i++) {
        std::cout<<i<<"<:"<<confusionSets.size()<<std::endl;


        if (i/100>progression){progression=i/100;std::cout<<progression*100<<"/"<<confusionSets.size()<<std::endl;}

        Point nodePoint=confusionSets[i].point();


        // If patch is entirely inside the mask the initial discimination is impossible therefore we let the confusion set empty (<==> confusion sets are all the labels)
        if (imageMask.isPatchInsideMask(nodePoint,patchSize)) {//optimisation possible since
            continue;
        }



        for (int x=patchSize/2;x<imageMaskExtended.width()-patchSize/2;x++) {
            for (int y=patchSize/2;y<imageMaskExtended.height()-patchSize/2;y++){


                Point labelPoint(x,y);
                int potential=imageInput.ssdMask(nodePoint,labelPoint,imageMask,patchSize);
                int currentBelief=-potential;
                Label currentLabel(labelPoint,potential);


                if (imageMaskExtended(x,y)<128){ //black pixel ==> possible label candidate



                    //If the size condition is met we start to discriminate the patches
                    //if (potential<thresholdConfusion){
                    if (currentBelief>confusionSets[i].worstBelief()){ //suppose is is sorted
                        if ( confusionSets[i].similarityCondition(imageInput,labelPoint,thresholdSimilarity,patchSize)){

                            confusionSets[i].pushConditioned(currentLabel,lmin,lmax);
                        }
                    }

                }

            }

        }
        //we then apply the confusion threshold on the remaining confusionset
        confusionSets[i].thresholdConfusion(thresholdConfusion,lmin);



    }


    return confusionSets;
}
