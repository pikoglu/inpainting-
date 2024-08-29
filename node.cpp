#include "node.h"
#include <fstream>

#include <numeric>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <algorithm>
#include <omp.h>



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

    // We brp
    for (int y=0;y<imageMask.height()+patchsize/2;y+=patchsize/2){
        for (int x=0;x<imageMask.width()+patchsize/2;x+=patchsize/2){
            if (y<imageMask.height() && x<imageMask.width()){
                if (imageMask(x,y,0)>128){
                    Point p(x,y);

                    Node n(index,p,lmax);
                    index++;
                    v.push_back(n);

                    //is there an already existing neighbors at the left ???
                    for (size_t i=0; i<v.size();i++){
                        if (v[i].getx()==x-patchsize/2 && v[i].gety()==y){

                            v.back().addLeftNeighbor(int(i));
                            v[i].addRightNeighbor(int(v.size()-1));
                            break;

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
    }


    return  v;
}

bool sortArgbelief(std::pair<Label,Point> a,std::pair<Label,Point> b){
    return a.first.belief()>b.first.belief();
}

bool Node::similarityCondition(const Image &imageInput, const Label labelCandidate, Point xq,
                               int thresholdSimilarity , int patchSize){
    Point pointLabel=labelCandidate.point();
    for (size_t i=0;i<this->size();i++){
        if (imageInput.ssd(pointLabel,label(i).point(),patchSize)<thresholdSimilarity){
            //swap if better
            if (labelCandidate.belief()>label(i).belief()){
                nodeConfusionSet.push_back(std::pair<Label,Point>(labelCandidate,xq));
                nodeConfusionSet.erase(nodeConfusionSet.begin()+i);
                std::sort(nodeConfusionSet.begin(),nodeConfusionSet.end(),sortArgbelief);


            return false;
            }
            return true;
        }
    }
    return true;
}

bool Node::pushConditioned(const Image &imageInput, const Label& label, int lmin, int lmax, int thresholdSimilarity, int patchSize, Point xq) {
    //If the min size is not yet reached we add labels to the confusion set no matter what
    bool pushed=false;
    if (nodeConfusionSet.size()<lmin){
        nodeConfusionSet.push_back(std::pair<Label,Point>(label,xq));
        pushed=true;

    }
    else{
        //If the max size is not yet reached we add label is they are not too similar to the already existing
        if (nodeConfusionSet.size()<lmax && (*this).similarityCondition(imageInput,label,xq,thresholdSimilarity,patchSize)){
            nodeConfusionSet.push_back(std::pair<Label,Point>(label,xq));

            pushed=true;
            if (nodeConfusionSet.size()==lmax)
                std::sort(nodeConfusionSet.begin(),nodeConfusionSet.end(),sortArgbelief);
        }
        //is the max size if reached we add label if they are more believable than the worst label and not too similar to the already existing
        else if (label.belief()>nodeConfusionSet.back().first.belief() && (*this).similarityCondition(imageInput,label,xq,thresholdSimilarity,patchSize) ){

            pushed=true;
            nodeConfusionSet.pop_back();

            if (label.belief()<=nodeConfusionSet.back().first.belief()){
                nodeConfusionSet.push_back(std::pair<Label,Point>(label,xq));
            }
            else{
                for (size_t i=0;i<nodeConfusionSet.size();i++){
                    if (nodeConfusionSet[i].first.belief()<label.belief()){
                        nodeConfusionSet.insert(nodeConfusionSet.begin()+i,std::pair<Label,Point>(label,xq));
                        break;
                    }
                }
            }
        }
    }
    return pushed;
}

Label Node::label(int i) const{
    assert(i<nodeConfusionSet.size());
    return nodeConfusionSet[i].first;}



void Node::pruningThresholdConfusion(int thresholdConfusion,int lmin){

    if (nodeConfusionSet.size() > lmin) {
        bool breaked=false;
        int maxBelief = nodeConfusionSet[0].first.belief();

        size_t validIndex = 0;
        for (size_t i = 0; i < nodeConfusionSet.size(); ++i) {
            if (nodeConfusionSet[i].first.belief() - maxBelief < thresholdConfusion) {
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
    return nodeConfusionSet.back().first.belief() ;}

Image visualiseNodesAndVertices(Image const &imageMask, std::vector<Node> v,int patchsize) {
    Image nodesAndVertices(imageMask.width(),imageMask.height(),3);

    for(int x=0;x<imageMask.width();x++){
        for(int y=0;y<imageMask.height();y++){
            nodesAndVertices(x,y,0)=imageMask(x,y,0);
            nodesAndVertices(x,y,1)=imageMask(x,y,0);
            nodesAndVertices(x,y,2)=imageMask(x,y,0);
        }
    }


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


Image visualiseMaskOverImage(Image const &imageInput,Image const &imageMask) {
    Image maskOverImage(imageInput.clone());
    for (int x=0;x<imageMask.width();x++){
        for (int y=0;y<imageMask.height();y++){
            if (imageMask(x,y,0)>128){
                maskOverImage(x,y,0)=255;
                maskOverImage(x,y,1)=255;
                maskOverImage(x,y,2)=255;
            }
        }
    }
    return maskOverImage;
}

bool sortArgSize(const Node &a,const Node &b){
    if (a.size()==0) return false;
    if (b.size()==0) return true;
    return a.size()<b.size();
}

void saveNodeCandidate(const Node & nodeCandidate,const Image& imageInput,
                       int patchSize, std::string path,int r,int s,int m){
    std::ofstream outFile;

    if (nodeCandidate.size()>0){

        Image candidates=visualizeCandidate(nodeCandidate,imageInput,patchSize);

        std::string name=std::string(path+"/candidates/"+
                                       std::to_string(r)+"R_"+
                                       std::to_string(s)+"S_"+
                                       std::to_string(nodeCandidate.getIndex())+"I_"+
                                       std::to_string(nodeCandidate.getx())+"X_"+
                                       std::to_string(nodeCandidate.gety())+"Y_"+
                                       std::to_string(m)+"M.png");



        if (!save_image(name.c_str(),candidates)){
            std::cerr << "Error writing file "+name << std::endl;
        }

        outFile.open(std::string(path+"/candidates/"+
                                 std::to_string(r)+"R_"+
                                 std::to_string(s)+"S_"+
                                 std::to_string(nodeCandidate.getIndex())+"I_"+
                                 std::to_string(nodeCandidate.getx())+"X_"+
                                 std::to_string(nodeCandidate.gety())+"Y_"+
                                 std::to_string(m)+"M.txt").c_str(),std::ios::out);

        for (size_t i=0;i<nodeCandidate.size();i++){
            outFile<<nodeCandidate.label(i).point().first<<' '<<nodeCandidate.label(i).point().second<<' '<<nodeCandidate.label(i).belief()<<std::endl;

        }


        outFile.close();
    }

}





std::vector<Node> assignInitialPriority( const Image& imageInput,const Image& imageMaskExtended,const Image& imageMask,
                                                       int patchSize, int lmin, int lmax,
                                        int thresholdConfusion, int thresholdSimilarity, std::string path) {


    std::vector<Node> confusionSets=nodesOverMask(imageMaskExtended,patchSize,lmax);



    int progression=0;

    // Iterate over each node
    for (size_t i=0;i<confusionSets.size();i++) {


        if (i/100>progression){progression=i/100;std::cout<<progression*100<<"/"<<confusionSets.size()<<std::endl;}

        Point nodePoint=confusionSets[i].point();


        // If patch is entirely inside the mask the initial discimination is impossible therefore we let the confusion set empty (<==> confusion sets are all the labels)
        if (imageMask.isPatchInsideMask(nodePoint,patchSize)) {//optimisation possible since
            continue;
        }




        for (int x=patchSize/2;x<imageMaskExtended.width()-patchSize/2;x++) {
            for (int y=patchSize/2;y<imageMaskExtended.height()-patchSize/2;y++){





                if (imageMaskExtended(x,y,0)<128){ //black pixel ==> possible label candidate


                    Point labelPoint(x,y);

                    int potential=imageInput.ssdMask(nodePoint,labelPoint,imageMask,patchSize);


                    Label currentLabel(labelPoint,potential);

                    if (false){
                        std::cout<<i<<"/"<<x<<"/"<<y<< "/"<<potential<<std::endl;
                    }


                    bool pushedVoid;
                    Point xq(0,0);

                    pushedVoid=confusionSets[i].pushConditioned(imageInput,currentLabel,lmin,lmax,thresholdSimilarity,patchSize,xq);


                }

            }

        }
        saveNodeCandidate(confusionSets[i],imageInput,patchSize,path,0,0,-1);

        confusionSets[i].pruningThresholdConfusion(thresholdConfusion,lmin);


        assert(confusionSets[i].size()>=lmin);
        assert(confusionSets[i].size()<=lmax);
        if (confusionSets[i].size()==0){std::cout<<"attention"<<std::endl;}



    }

    std::sort(confusionSets.begin(),confusionSets.end(),sortArgSize);
    return confusionSets;
}

bool Node::inStack(const std::vector<int> &stack){
    for (size_t i=0;i<stack.size();i++){
        if (stack[i]==index){
            return true;
        }
    }
    return false;
}

std::vector<bool> intializeFalseVector(int size){
    std::vector<bool> commitList;
    for (int i=0;i<size;i++){
        commitList.push_back(false);
    }
    return commitList;

}

std::vector<bool> intializeTrueVector(int size){
    std::vector<bool> commitList;
    for (int i=0;i<size;i++){
        commitList.push_back(true);
    }
    return commitList;

}




std::pair<double,Point> Node::messageReceived(  const Node  &sender, const Point &coordPatchCandidate, const Image &imageInput,int patchSize,int w0){
    //correspond to the message reveived by this from sender by choosing coordPatchCandidate as patch
    double bestMessage=std::numeric_limits<double>::max();
    std::vector<std::pair<Label,Point> > confusionSetSender=sender.getNodeConfusionSet();
    Point bestxq;
    for (size_t i=0;i<confusionSetSender.size();i++){

        Label labelSender=confusionSetSender[i].first;
        double messageSent=labelSender.getPotential();
        messageSent+=imageInput.ssdOverlap(sender.point(),(*this).point(),labelSender.point(),coordPatchCandidate,patchSize,w0);

        assert(messageSent>=0);

        if (sender.point().first == patchSize/2+ (*this).point().first) { // Message to the left
            messageSent += labelSender.getMessageFromRight();
            assert(labelSender.getMessageFromRight()>=0);
            messageSent += labelSender.getMessageFromTop();
            assert(labelSender.getMessageFromTop()>=0);
            messageSent += labelSender.getMessageFromBottom();
            assert(labelSender.getMessageFromBottom()>=0);
        }
        else if (sender.point().first +patchSize/2== (*this).point().first) { // Message to the right
            messageSent += labelSender.getMessageFromLeft();
            assert( labelSender.getMessageFromLeft()>=0);
            messageSent += labelSender.getMessageFromTop();
            assert(labelSender.getMessageFromTop()>=0);
            messageSent += labelSender.getMessageFromBottom();
            assert(labelSender.getMessageFromBottom()>=0);
        }
        else if (sender.point().second ==patchSize/2+ (*this).point().second) { // Message to the bottom
            messageSent += labelSender.getMessageFromLeft();
            assert( labelSender.getMessageFromLeft()>=0);
            messageSent += labelSender.getMessageFromRight();

            assert( labelSender.getMessageFromRight()>=0);
            messageSent += labelSender.getMessageFromTop();
            assert(labelSender.getMessageFromTop()>=0);
        }
        else if (sender.point().second +patchSize/2== (*this).point().second) { // Message to the top
            messageSent += labelSender.getMessageFromLeft();
            assert( labelSender.getMessageFromLeft()>=0);
            messageSent += labelSender.getMessageFromRight();
            assert( labelSender.getMessageFromRight()>=0);
            messageSent += labelSender.getMessageFromBottom();
            assert( labelSender.getMessageFromBottom()>=0);
        }
        else{
            std::cout<<"message not from neigbhbor messageReceived"<<std::endl;
        }
        //std::cout<<i<<'/'<<messageSent<<std::endl;
        if (bestMessage>messageSent){
            bestMessage=messageSent;
            bestxq=confusionSetSender[i].first.point();
        }

        //std::cout<<i<<'/'<<bestMessage<<std::endl;

    }

    return  std::pair<double,Point> (bestMessage,bestxq);
}


//Paralelisation


//fin paralelisation


void Node::createNodeConfusionSet(const Node &sender,
                                  const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,
                                  int thresholdConfusion,int lmin,int lmax,
                                  std::string path,int s,int w0){

    if (sender.size()<lmin){
        std::cout<<"createNodeConfusionSet"<<std::endl;
        assert(sender.size()>=lmin);
    }
    assert((*this).getIndex()!=sender.getIndex());

    std::vector<std::pair<Label,Label> > labelsPushed;


    for (int x=patchSize/2;x<imageMaskExtended.width()-patchSize/2;x++) {
        for (int y=patchSize/2;y<imageMaskExtended.height()-patchSize/2;y++){

            if (imageMaskExtended(x,y,0)<128){ //black pixel ==> possible label candidate

                Point labelPoint(x,y);
                Label currentLabel(labelPoint,0);
                std::pair<double,Point> messageAndPointFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize,w0);
                double messageFromSender=messageAndPointFromSender.first;
                if ( messageFromSender<0){
                    assert(messageFromSender>=0);

                }

                if (sender.point().first==patchSize/2+(*this).point().first){//Message from the right
                    currentLabel.setMessageFromRight(messageFromSender);
                }
                else if(sender.point().first+patchSize/2==(*this).point().first){//Message from the left
                    currentLabel.setMessageFromLeft(messageFromSender);
                }
                else if(sender.point().second==patchSize/2+(*this).point().second){//Message from the top
                    currentLabel.setMessageFromTop(messageFromSender);
                }
                else if (sender.point().second+patchSize/2==(*this).point().second) {//Message from the bottom
                    currentLabel.setMessageFromBottom(messageFromSender);
                }
                else{
                    std::cout<<"message node from neighbor createNodeConfusionSet"<<std::endl;
                }


                bool pushed;

                pushed=(*this).pushConditioned(imageInput,currentLabel,lmin,lmax,thresholdSimilarity,patchSize,messageAndPointFromSender.second);


            }

        }

    }

    if ((*this).size()<lmin){
        std::cout<<"createNodeConfusionSet"<<std::endl;
        assert((*this).size()>=lmin);
    }

    (*this).pruningThresholdConfusion(thresholdConfusion,lmin);

    saveNodeCandidate((*this),imageInput,patchSize,path,1,s,sender.getIndex());



}

void Node::updateNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,
                                  const Image &imageInput,int patchSize,int thresholdSimilarity,
                                  int thresholdConfusion,int lmin,int lmax,std::string path,int s,int w0){

    assert((*this).size()>0);
    if ((*this).size()<lmin){
        std::cout<<"updateNodeConfusionSet"<<std::endl;
        assert((*this).size()>=lmin);
    }
    assert(sender.size()>0);
    if (sender.size()<lmin){
        std::cout<<"updateNodeConfusionSet"<<std::endl;
        assert(sender.size()>=lmin);
    }
    std::vector<std::pair<Label,Point> > & receiverNodeConfusionSet=this->getNodeConfusionSet();

    for (size_t i=0;i<(*this).size();i++) {

            Point labelPoint(receiverNodeConfusionSet[i].first.point());
            std::pair<double,Point> messageAndPointFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize,w0);
            double messageFromSender=messageAndPointFromSender.first;
            if (messageFromSender<0){
                std::cout<<messageFromSender<<std::endl;
                assert(messageFromSender>=0);
            }
            if (sender.point().first>(*this).point().first){//Message from the right
                //std::cout<<"avant"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
                receiverNodeConfusionSet[i].first.setMessageFromRight(messageFromSender);//pos potential left right top botom
                //std::cout<<"apres"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
            }
            else if(sender.point().first<(*this).point().first){//Message from the left
                receiverNodeConfusionSet[i].first.setMessageFromLeft(messageFromSender);
            }
            else if(sender.point().second>(*this).point().second){//Message from the top
                receiverNodeConfusionSet[i].first.setMessageFromTop(messageFromSender);
            }
            else if (sender.point().second<(*this).point().second) {//Message from the bottom
                receiverNodeConfusionSet[i].first.setMessageFromBottom(messageFromSender);
            }
            else{
                std::cout<<"message not from neighbor updateNodeConfusionSet"<<std::endl;
            }

    }

    std::sort(receiverNodeConfusionSet.begin(),receiverNodeConfusionSet.end(),sortArgbelief);

    (*this).pruningThresholdConfusion(thresholdConfusion,lmin);

    saveNodeCandidate(*this,imageInput,patchSize,path,1,s,sender.getIndex());





}



void Node::updateNodeConfusionSetNoPruning(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,
                                           int patchSize,int thresholdSimilarity,int thresholdConfusion,
                                           int lmin,int lmax,std::string path,int s,int w0){
    assert((*this).size()>0);
    if ((*this).size()<lmin){
        std::cout<<"updateNodeConfusionSet"<<std::endl;
        assert((*this).size()>=lmin);
    }
    assert(sender.size()>0);
    if (sender.size()<lmin){
        std::cout<<"updateNodeConfusionSet"<<std::endl;
        assert(sender.size()>=lmin);
    }
    std::vector<std::pair<Label,Point> > & receiverNodeConfusionSet=this->getNodeConfusionSet();

    for (size_t i=0;i<(*this).size();i++) {

        Point labelPoint(receiverNodeConfusionSet[i].first.point());
        std::pair<double,Point> messageAndPointFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize,w0);
        double messageFromSender=messageAndPointFromSender.first;
        if (messageFromSender<0){
            std::cout<<messageFromSender<<std::endl;
            assert(messageFromSender>=0);
        }
        if (sender.point().first>(*this).point().first){//Message from the right
            //std::cout<<"avant"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
            receiverNodeConfusionSet[i].first.setMessageFromRight(messageFromSender);//pos potential left right top botom
            //std::cout<<"apres"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
        }
        else if(sender.point().first<(*this).point().first){//Message from the left
            receiverNodeConfusionSet[i].first.setMessageFromLeft(messageFromSender);
        }
        else if(sender.point().second>(*this).point().second){//Message from the top
            receiverNodeConfusionSet[i].first.setMessageFromTop(messageFromSender);
        }
        else if (sender.point().second<(*this).point().second) {//Message from the bottom
            receiverNodeConfusionSet[i].first.setMessageFromBottom(messageFromSender);
        }
        else{
            std::cout<<"message not from neighbor updateNodeConfusionSet"<<std::endl;
        }

    }
    std::sort(receiverNodeConfusionSet.begin(),receiverNodeConfusionSet.end(),sortArgbelief);
    saveNodeCandidate(*this,imageInput,patchSize,path,2,s,sender.getIndex());



}




void updatePriority(size_t index, std::vector<Node> &InitialPriority) {
    if (index >= InitialPriority.size()) {
        std::cout<<"index to high updatePriority"<<std::endl;
        return;
    }

    Node nodeToMove = InitialPriority[index];

    // Find the correct position for the node
    std::vector<Node>::iterator it = std::lower_bound(InitialPriority.begin(), InitialPriority.end(), nodeToMove, sortArgSize);

    // If the node is already in the correct position
    if (it - InitialPriority.begin() == static_cast<long>(index)) {
        return;
    }

    InitialPriority.erase(InitialPriority.begin() + index);

    InitialPriority.insert(it, nodeToMove);
}

int commitListSum(const std::vector<bool> &commitStack){
    int sum=0;
    for (size_t i=0;i<commitStack.size();i++){
        if (commitStack[i]){sum++;}
    }
    return sum;
}

size_t getNodeOfIndex(const std::vector<Node>& InitialPriority, int j)  {
    for (size_t i = 0; i < InitialPriority.size(); i++) {
        if (InitialPriority[i].getIndex() == j) {
            return i;
        }
    }
    throw std::runtime_error("Node not found");//en ajouter partour
}

size_t getNodeOfCoord(const std::vector<Node>& InitialPriority, int x,int y) {
    for (size_t i = 0; i < InitialPriority.size(); i++) {
        if (InitialPriority[i].getx() == x && InitialPriority[i].gety() == y) {
            return InitialPriority[i].getIndex();
        }
    }
    throw std::runtime_error("Node not found");//en ajouter partour
}


std::vector<int> forwardPass(std::vector<Node> &InitialPriority, const Image &imageInput,
                             const Image &imageMaskExtended, Image &orderOfVisit,
                             int patchSize, int thresholdSimilarity, int thresholdConfusion, int lmin,
                             int lmax, std::string path,int w0){
    std::vector<int> commitStack;
    std::vector<bool> commitList=intializeFalseVector(InitialPriority.size());
    orderOfVisit=imageMaskExtended.clone();
    size_t indexCommitSearch;
    int progression=0;
    std::cout<<"initial priority size "<<InitialPriority.size()<<std::endl;
    for (size_t time=0;time<InitialPriority.size();time++){

        if (time/100>progression){
            progression++;
            std::cout<<time<<"/"<<InitialPriority.size() <<std::endl;
        }
        indexCommitSearch=0;
        while (commitList[InitialPriority[indexCommitSearch].getIndex()]){
            indexCommitSearch++;
        }
        Node currentNode(InitialPriority[indexCommitSearch]);

        commitStack.push_back(currentNode.getIndex());
        commitList[InitialPriority[indexCommitSearch].getIndex()]=true;

        //Temporaire
        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){
                if (currentNode.getx() +x>=0 && currentNode.getx() +x<orderOfVisit.width() && currentNode.gety()+y>=0 && currentNode.gety()+y<orderOfVisit.height()){
                    orderOfVisit(currentNode.getx() +x,currentNode.gety()+y,0)=int((float(time)/float(InitialPriority.size()))*255);
                }

            }

        }

        //fin temporaire


        if (currentNode.hasLeftNeighbor() && not commitList[currentNode.getLeftNeighbor()]) {

            int indexLeftNeighbor = getNodeOfIndex(InitialPriority, currentNode.getLeftNeighbor());


            if (InitialPriority[indexLeftNeighbor].size() == 0) { // interior
                InitialPriority[indexLeftNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput,
                                                                          patchSize, thresholdSimilarity, thresholdConfusion,
                                                                          lmin, lmax,path,time,w0);
            }
            else {
                InitialPriority[indexLeftNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize,
                                                                          thresholdSimilarity, thresholdConfusion,
                                                                          lmin, lmax,path,time,w0);
            }
        }

        if (currentNode.hasRightNeighbor() && not commitList[currentNode.getRightNeighbor()]) {
            int indexRightNeighbor = getNodeOfIndex(InitialPriority, currentNode.getRightNeighbor());
            if (InitialPriority[indexRightNeighbor].size() == 0) { // interior
                InitialPriority[indexRightNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize,
                                                                           thresholdSimilarity, thresholdConfusion,
                                                                           lmin, lmax,path,time,w0);
            }
            else {

                InitialPriority[indexRightNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize,
                                                                           thresholdSimilarity, thresholdConfusion,
                                                                           lmin, lmax,path,time,w0);

            }
        }

        if (currentNode.hasTopNeighbor() && not commitList[currentNode.getTopNeighbor()]) {
            int indexTopNeighbor = getNodeOfIndex(InitialPriority, currentNode.getTopNeighbor());

            if (InitialPriority[indexTopNeighbor].size() == 0) { // interior
                InitialPriority[indexTopNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize,
                                                                         thresholdSimilarity, thresholdConfusion,
                                                                         lmin, lmax,path,time,w0);
            }
            else {
                InitialPriority[indexTopNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput,
                                                                         patchSize, thresholdSimilarity, thresholdConfusion,
                                                                         lmin, lmax,path,time,w0);
            }
        }

        if (currentNode.hasBottomNeighbor() && not commitList[currentNode.getBottomNeighbor()]) {
            int indexBottomNeighbor = getNodeOfIndex(InitialPriority, currentNode.getBottomNeighbor());

            if (InitialPriority[indexBottomNeighbor].size() == 0) { // interior
                InitialPriority[indexBottomNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput,
                                                                            patchSize, thresholdSimilarity, thresholdConfusion,
                                                                            lmin, lmax,path,time,w0);
            }
            else {
                InitialPriority[indexBottomNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput,
                                                                            patchSize, thresholdSimilarity, thresholdConfusion,
                                                                            lmin, lmax,path,time,w0);
            }
        }
        std::sort(InitialPriority.begin(),InitialPriority.end(),sortArgSize);
    }
    return commitStack;
}

Image imageReconstructed(const std::vector<Node> &InitialPriority, int patchSize,Image inputImage,Image maskImage){
    Image imageReconstructed=inputImage.clone();


    for (size_t i = InitialPriority.size(); i-- > 0; ) {//cor
        if (true){//InitialPriority[i].size()>1){
        Point p=InitialPriority[i].point();
        Label l=InitialPriority[i].getBestLabel();
        Point lp=l.point();

        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){
                if (p.first+x>0 && p.first+x<inputImage.width() && p.second +y>0 && p.second +y<inputImage.height()){
                    imageReconstructed(p.first+x,p.second +y,0)=inputImage(lp.first+x,lp.second+y,0);
                    imageReconstructed(p.first+x,p.second+y,1)=inputImage(lp.first+x,lp.second+y,1);
                    imageReconstructed(p.first+x,p.second+y,2)=inputImage(lp.first+x,lp.second+y,2);
                }

            }
        }
    }

    for (size_t i = InitialPriority.size(); i-- > 0; ) {
        Point p=InitialPriority[i].point();
        for (int x=-patchSize/2;x<=patchSize/2;x++){
                if (p.first+x>=0 && p.first+x<inputImage.width()){


                    imageReconstructed(p.first+x,p.second,0)=0;
                    imageReconstructed(p.first+x,p.second,1)=255;
                    imageReconstructed(p.first+x, p.second, 2) = 255;}

        }
        for (int y=-patchSize/2;y<=patchSize/2;y++){
                if (p.second+y>=0 && p.second+y<inputImage.height()){
                imageReconstructed(p.first,p.second+y,0)=0;
                imageReconstructed(p.first,p.second+y,1)=255;
                imageReconstructed(p.first,p.second+y,2)=255;}
        }
    }


    }


    return imageReconstructed;
}


Image visualizeCandidate(const Node &nodeCandidate,const Image &imageInput,int patchSize){

    int size=nodeCandidate.size();
    Image candidate(size*patchSize,patchSize,3);

    for (int i=0;i<size;i++){

        Label labelCandidate=nodeCandidate.label(i);
        Point p=labelCandidate.point();
        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){

                if (p.first+x>=0 && p.first+x<imageInput.width() && p.second+y>=0 && p.second+y<imageInput.height()){
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,0)=imageInput(p.first+x,p.second+y,0);
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,1)=imageInput(p.first+x,p.second+y,1);
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,2)=imageInput(p.first+x,p.second+y,2);
                }
                else{
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,0)=0;
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,1)=0;
                    candidate(i*patchSize+x+patchSize/2,y+patchSize/2,2)=0;
                }
            }
        }
    }
    return candidate;

}


Image backwardPass(std::vector<Node> &InitialPriority,std::vector<int> commitStack,const Image &imageInput, const Image &imageMaskExtended,int patchSize,int thresholdSimilarity,int thresholdConfusion,
                   int lmin,int lmax,std::string path,int w0){

    std::vector<bool> commitList=intializeTrueVector(InitialPriority.size());
    Image orderOfVisit=imageMaskExtended.clone();
    int index;
    int progression=0;
    for (size_t time=0;time<InitialPriority.size();time++){

        if (time/100>progression){
            progression++;
            std::cout<<time<<"/"<<InitialPriority.size() <<std::endl;
        }
        index=commitStack.back();
        commitStack.pop_back();

        Node currentNode(InitialPriority[getNodeOfIndex(InitialPriority,index)]);






        commitList[index]=false;


        //Temporaire
        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){
                if (currentNode.getx() +x>=0 && currentNode.getx() +x<orderOfVisit.width() && currentNode.gety()+y>=0 && currentNode.gety()+y<orderOfVisit.height())
                orderOfVisit(currentNode.getx() +x,currentNode.gety()+y,0)=int((float(time)-1)/float(InitialPriority.size())*255);
            }

        }

        //fin temporaire


        if (currentNode.hasLeftNeighbor() &&  commitList[currentNode.getLeftNeighbor()]) {

            int indexLeftNeighbor = getNodeOfIndex(InitialPriority, currentNode.getLeftNeighbor());

            //No pruning in backward pass cf article
            InitialPriority[indexLeftNeighbor].updateNodeConfusionSetNoPruning(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity,
                                                                               thresholdConfusion, lmin, lmax,path,time,w0);

        }

        if (currentNode.hasRightNeighbor() &&  commitList[currentNode.getRightNeighbor()]) {
            int indexRightNeighbor = getNodeOfIndex(InitialPriority, currentNode.getRightNeighbor());


            InitialPriority[indexRightNeighbor].updateNodeConfusionSetNoPruning(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity,
                                                                                thresholdConfusion, lmin, lmax,path,time,w0);

        }

        if (currentNode.hasTopNeighbor() &&  commitList[currentNode.getTopNeighbor()]) {
            int indexTopNeighbor = getNodeOfIndex(InitialPriority, currentNode.getTopNeighbor());



                InitialPriority[indexTopNeighbor].updateNodeConfusionSetNoPruning(currentNode, imageMaskExtended, imageInput, patchSize,
                                                                              thresholdSimilarity, thresholdConfusion, lmin, lmax,path,time,w0);

        }

        if (currentNode.hasBottomNeighbor() &&  commitList[currentNode.getBottomNeighbor()]) {
            int indexBottomNeighbor = getNodeOfIndex(InitialPriority, currentNode.getBottomNeighbor());



                InitialPriority[indexBottomNeighbor].updateNodeConfusionSetNoPruning(currentNode, imageMaskExtended, imageInput,
                                                                                 patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax,path,time,w0);

        }
        std::sort(InitialPriority.begin(),InitialPriority.end(),sortArgSize);
    }
    return orderOfVisit;
}




Image labelRepartition(const std::vector<Node>& priority,int lmax){
    int widthBar=10;
    Image labelRepartition(lmax*widthBar,lmax,1);
    //set labelRepartition to white
    for (int x=0;x<labelRepartition.width();x++){
        for (int y=0;y<labelRepartition.width();y++){
            labelRepartition(x,y,0)=0;

        }
    }

    int sizes[lmax];
    for (int i=0;i<lmax;i++){
        sizes[i]=0;
    }
    for (size_t i=0;i<priority.size();i++){
        sizes[priority[i].size()]+=1;
    }
    for (int i=0;i<lmax;i++){
        for (int j=0;j<sizes[i];j++){
            for (int x=i*widthBar;x<(i+1)*widthBar;x++){
                labelRepartition(x,j,0)=255;
            }

        }
    }
    return labelRepartition;
}



void pourcentageNoeudPruned(const std::vector<Node>& priorities,int lmax,int lmin){
    float pruned=0.0;
    float non_pruned=0.0;

    float tailleMoyenne=0.0;

    for (size_t i=0;i<priorities.size();i++){
        if (priorities[i].size()<lmax && priorities[i].size()>=lmin){
            pruned++;}
        if (priorities[i].size()>0){
            tailleMoyenne+=priorities[i].size();
            non_pruned++;

        }
    }


    std::cout<<"pourcentage de noeud pruned : "<<float(pruned)/float(non_pruned)<<std::endl;
    std::cout<<"taille moyenne non vide : "<<float(tailleMoyenne)/float(non_pruned)<<std::endl;


}


Image getConfusionSet(const std::vector<Node>& priorities,const Image& imageInput,
                      int patchSize,int lmax){

    Image confusionSet(imageInput.clone());

    for (size_t i=0;i<priorities.size();i++){
        int x=priorities[i].getx();
        int y=priorities[i].gety();
        int size=priorities[i].size();
        if (size==0){
            for (int xp=x-patchSize/2;xp<=x+patchSize/2;xp++){
                for (int yp=y-patchSize/2;yp<=y+patchSize/2;yp++){
                    if (xp>=0 && xp<imageInput.width() && yp>=0 && yp<imageInput.height()){
                        confusionSet(xp,yp,0)=0;
                        confusionSet(xp,yp,1)=0;
                        confusionSet(xp,yp,2)=255;
                    }
                }
            }
        }

    }

    for (size_t i=0;i<priorities.size();i++){
        int x=priorities[i].getx();
        int y=priorities[i].gety();
        int size=priorities[i].size();

        if (size>0){
            for (int xp=x-patchSize/2;xp<=x+patchSize/2;xp++){
                for (int yp=y-patchSize/2;yp<=y+patchSize/2;yp++){
                    if (xp>=0 && xp<imageInput.width() && yp>=0 && yp<imageInput.height()){
                        confusionSet(xp,yp,0)=255-(size/lmax)*255;
                        confusionSet(xp,yp,1)=0;
                        confusionSet(xp,yp,2)=0;
                    }
                }
            }
        }

    }
    return confusionSet;

}


Image imageReconstructedBlend(const std::vector<Node>& InitialPriority, int patchSize, Image inputImage, Image maskImage,int lmax) {
    // Create a 16-channel image for blending (4 * RGBW)
    Image blendedImage(inputImage.width(), inputImage.height(), 16);

    // Initialize blendedImage with zeros
    for (int x = 0; x < blendedImage.width(); x++) {
        for (int y = 0; y < blendedImage.height(); y++) {
            for (int channel = 0; channel < blendedImage.channels(); channel++) {
                blendedImage(x, y, channel) = 0;
            }
        }
    }

    // Process nodes and accumulate color and weight information
    for (const Node& node : InitialPriority) {
        Point p = node.point();
        const std::vector<std::pair<Label, Point>>& confusionSet = node.getNodeConfusionSet();

        if (!confusionSet.empty()) {
            Label bestLabel = confusionSet[0].first;
            Point lp = bestLabel.point();
            unsigned char confidence = lmax - confusionSet.size()>255? static_cast<unsigned char>(255): static_cast<unsigned char>(lmax - confusionSet.size());



            bool blendLeft = node.hasLeftNeighbor();
            bool blendTop = node.hasTopNeighbor();
            bool blendRight = node.hasRightNeighbor();
            bool blendBottom = node.hasBottomNeighbor();

            for (int x = -patchSize / 2; x <= patchSize / 2; x++) {
                for (int y = -patchSize / 2; y <= patchSize / 2; y++) {
                    int px = p.first + x;
                    int py = p.second + y;
                    int lpx = lp.first + x;
                    int lpy = lp.second + y;



                    if (px >= 0 && px < blendedImage.width() && py >= 0 && py < blendedImage.height()) {

                        int channelOffset = 0;
                        if (blendLeft && x < 0) channelOffset = 0;
                        else if (blendTop && y < 0) channelOffset = 4;
                        else if (blendRight && x > 0) channelOffset = 8;
                        else if (blendBottom && y > 0) channelOffset = 12;
                        else continue;

                        // Accumulate color values and weight
                        for (int channel = 0; channel < 3; channel++) {
                            blendedImage(px, py, channelOffset + channel) = inputImage(lpx, lpy, channel);

                        }


                        blendedImage(px, py, channelOffset + 3) = confidence;
                    }
                }
            }
        }
    }

    // Create the final RGB image
    Image finalImage(inputImage.width(), inputImage.height(), 3);

    // Perform weighted average and insert into final image
    for (int x = 0; x < finalImage.width(); x++) {
        for (int y = 0; y < finalImage.height(); y++) {
            float totalWeight = 0;
            float r = 0, g = 0, b = 0;

            // Calculate weighted average from the 4 neighbors
            for (int i = 0; i < 4; i++) {
                int offset = i * 4;
                float weight = blendedImage(x, y, offset + 3);
                totalWeight += weight;
                r += blendedImage(x, y, offset) * weight;
                g += blendedImage(x, y, offset + 1) * weight;
                b += blendedImage(x, y, offset + 2) * weight;
            }

            // Normalize and assign to final image

            if (totalWeight > 0) {


                finalImage(x, y, 0) = static_cast<unsigned char>(r / totalWeight);
                finalImage(x, y, 1) = static_cast<unsigned char>(g / totalWeight);
                finalImage(x, y, 2) = static_cast<unsigned char>(b / totalWeight);

            } else {
                // Fallback to original image if no weight
                finalImage(x, y, 0) = inputImage(x, y, 0);
                finalImage(x, y, 1) = inputImage(x, y, 1);
                finalImage(x, y, 2) = inputImage(x, y, 2);
            }
        }
    }

    return finalImage;
}





