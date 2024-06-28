#include "node.h"
#include <algorithm>



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

Label Node::label(int i) const{
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



void Node::thresholdConfusion(int thresholdConfusion,int lmin){

    if (nodeConfusionSet.size() > lmin) {
        bool breaked=false;
        int maxBelief = nodeConfusionSet[0].belief();

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



bool sortArgSize(Node a,Node b){return a.size()<b.size();}

std::vector<Node> assignInitialPriority( const Image& imageInput,const Image& imageMaskExtended,const Image& imageMask,
                                                       int patchSize, int lmin, int lmax,
                                                       int thresholdConfusion, int thresholdSimilarity) {


    std::vector<Node> confusionSets=nodesOverMask(imageMask,patchSize,lmax);



    int progression=0;

    // Iterate over each node
    for (size_t i=0;i<confusionSets.size();i++) {
        //std::cout<<i<<"<:"<<confusionSets.size()<<std::endl;


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



int Node::messageReceived(  const Node  &sender, const Point &coordPatchCandidate, const Image &imageInput,int patchSize){
    int messageSent=0;
    int bestMessage=1000000;
    std::vector<Label> confusionSetSender=sender.getNodeConfusionSet();

    for (size_t i=0;i<confusionSetSender.size();i++){
        Label labelCandidate=confusionSetSender[i];
        messageSent=labelCandidate.getPotential();
        messageSent+=imageInput.ssdOverlap(sender.point(),(*this).point(),labelCandidate.point(),coordPatchCandidate,patchSize);
        if (sender.point().first > (*this).point().first) { // Message from the right
            messageSent += labelCandidate.getMessageFromRight();
            messageSent += labelCandidate.getMessageFromTop();
            messageSent += labelCandidate.getMessageFromBottom();
        }
        else if (sender.point().first < (*this).point().first) { // Message from the left
            messageSent += labelCandidate.getMessageFromLeft();
            messageSent += labelCandidate.getMessageFromTop();
            messageSent += labelCandidate.getMessageFromBottom();
        }
        else if (sender.point().second > (*this).point().second) { // Message from the top
            messageSent += labelCandidate.getMessageFromLeft();
            messageSent += labelCandidate.getMessageFromRight();
            messageSent += labelCandidate.getMessageFromTop();
        }
        else if (sender.point().second < (*this).point().second) { // Message from the bottom
            messageSent += labelCandidate.getMessageFromLeft();
            messageSent += labelCandidate.getMessageFromRight();
            messageSent += labelCandidate.getMessageFromBottom();
        }
        else{
            std::cout<<"index sender : "<<sender.index<<std::endl;
            std::cout<<"index receiver : "<<(*this).index<<std::endl;
            std::cout<<"message not from neigbhbor messageReceived"<<std::endl;
        }
        if (bestMessage>messageSent){
            bestMessage=messageSent;
        }
    }
    return  bestMessage;
}


bool sortArgbelief(Label a,Label b){
    return a.belief()>b.belief();
}
void Node::createNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
    assert((*this).size()==0);
    assert((*this).getIndex()!=sender.getIndex());
    for (int x=patchSize/2;x<imageMaskExtended.width()-patchSize/2;x++) {
        for (int y=patchSize/2;y<imageMaskExtended.height()-patchSize/2;y++){


            Point labelPoint(x,y);
            Label currentLabel(labelPoint,0);
            int messageFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize);

            if (sender.point().first>(*this).point().first){//Message from the right
                currentLabel.setMessageFromRight(messageFromSender);//pos potential left right top botoom
            }
            else if(sender.point().first<(*this).point().first){//Message from the left
                currentLabel.setMessageFromLeft(messageFromSender);
            }
            else if(sender.point().second>(*this).point().second){//Message from the top
                currentLabel.setMessageFromTop(messageFromSender);
            }
            else if (sender.point().second<(*this).point().second) {//Message from the bottom
                currentLabel.setMessageFromBottom(messageFromSender);
            }
            else{
                std::cout<<"message node from neighbor createNodeConfusionSet"<<std::endl;
                /*
                std::cout<<"index sender : "<<sender.getIndex()<<std::endl;
                std::cout<<"index receiver : "<<(*this).getIndex()<<std::endl;
                std::cout<<"x sender :"<<sender.point().first<<std::endl;
                std::cout<<"y sender :"<<sender.point().second<<std::endl;
                std::cout<<"x receiver :"<<(*this).point().first<<std::endl;
                std::cout<<"y receiver :"<<(*this).point().second<<std::endl;
                ;*/

            }



            if (imageMaskExtended(x,y)<128){ //black pixel ==> possible label candidate



                //If the size condition is met we start to discriminate the patches
                //if (potential<thresholdConfusion){
                if (currentLabel.belief()>(*this).worstBelief()){ //suppose is is sorted
                    if ( (*this).similarityCondition(imageInput,labelPoint,thresholdSimilarity,patchSize)){
                        (*this).pushConditioned(currentLabel,lmin,lmax);
                    }
                }

            }

        }

    }
     (*this).thresholdConfusion(thresholdConfusion,lmin);
}


void Node::updateNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
    assert((*this).size()>0);
    std::vector<Label> receiverNodeConfusionSet=(*this).getNodeConfusionSet();
    for (size_t i=0;i<(*this).size();i++) {


            Point labelPoint(receiverNodeConfusionSet[i].point());
            Label currentLabel(labelPoint,0);
            int messageFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize);
            if (sender.point().first>(*this).point().first){//Message from the right
                currentLabel.setMessageFromRight(messageFromSender);//pos potential left right top botoom
            }
            else if(sender.point().first<(*this).point().first){//Message from the left
                currentLabel.setMessageFromLeft(messageFromSender);
            }
            else if(sender.point().second>(*this).point().second){//Message from the top
                currentLabel.setMessageFromTop(messageFromSender);
            }
            else if (sender.point().second<(*this).point().second) {//Message from the bottom
                currentLabel.setMessageFromBottom(messageFromSender);
            }
            else{
                std::cout<<"message not from neighbor updateNodeConfusionSet"<<std::endl;
            }

        }
    std::sort(receiverNodeConfusionSet.begin(),receiverNodeConfusionSet.end(),sortArgbelief);
    (*this).thresholdConfusion(thresholdConfusion,lmin);

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

Node getNodeOfIndex(const std::vector<Node> &InitialPriority, int j ){
    for (size_t i =0;i<InitialPriority.size();i++){
        if (InitialPriority[i].getIndex()==j) return InitialPriority[i];
    }
    assert(false);
}

void forwardPass(std::vector<Node> &InitialPriority,const Image &imageInput, const Image &imageMaskExtended,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
    std::vector<int> commitStack;
    std::vector<bool> commitList=intializeFalseVector(InitialPriority.size());
    size_t indexCommitSearch;
    for (size_t time=0;time<InitialPriority.size();time++){
        std::cout<<"time: "<<time<<std::endl;

        indexCommitSearch=0;
        while (commitList[InitialPriority[indexCommitSearch].getIndex()]){
            indexCommitSearch++;
        }


        Node currentNode(InitialPriority[indexCommitSearch]);
        commitStack.push_back(currentNode.getIndex());
        commitList[InitialPriority[indexCommitSearch].getIndex()]=true;

        if (currentNode.hasLeftNeighbor() && not commitList[currentNode.getLeftNeighbor()]) {
            int indexLeftNeighbor = currentNode.getLeftNeighbor();
            Node leftNeighbor(getNodeOfIndex(InitialPriority,indexLeftNeighbor));
            if (leftNeighbor.size() == 0) { // interior
                leftNeighbor.createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                leftNeighbor.updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasRightNeighbor() && not commitList[currentNode.getRightNeighbor()]) {
            int indexRightNeighbor = currentNode.getRightNeighbor();
            Node rightNeighbor(getNodeOfIndex(InitialPriority,indexRightNeighbor));
            if (rightNeighbor.size() == 0) { // interior
                rightNeighbor.createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                rightNeighbor.updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasTopNeighbor() && not commitList[currentNode.getTopNeighbor()]) {
            int indexTopNeighbor = currentNode.getTopNeighbor();
            Node topNeighbor(getNodeOfIndex(InitialPriority,indexTopNeighbor));
            if (topNeighbor.size() == 0) { // interior
                topNeighbor.createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                topNeighbor.updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasBottomNeighbor() && not commitList[currentNode.getBottomNeighbor()]) {
            int indexBottomNeighbor = currentNode.getBottomNeighbor();
            Node bottomNeighbor(getNodeOfIndex(InitialPriority,indexBottomNeighbor));
            if (bottomNeighbor.size() == 0) { // interior
                bottomNeighbor.createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                bottomNeighbor.updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }
        std::sort(InitialPriority.begin(),InitialPriority.end(),sortArgSize);
    }
}


Image imageReconstructed(const std::vector<Node> &InitialPriority, int patchSize,Image inputImage,Image maskImage){
    std::cout<<"bonjour"<<std::endl;
    Image imageReconstructed=maskImage.clone();
    for (size_t i=0;i<InitialPriority.size();i++){
        std::cout<<i<<std::endl;
        if (InitialPriority[i].size()>1){
        Point p=InitialPriority[i].point();

        Label l=InitialPriority[i].label(0);
        Point lp=l.point();
        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){
                std::cout<<p.first+x<<"and"<<lp.first+x<<std::endl;
                if (p.first+x<imageReconstructed.width() && p.first+x>=0
                        && p.second+x<imageReconstructed.height() && p.first+x>=0
                        && lp.first+x<imageReconstructed.width() && lp.first+x>=0
                        && lp.second+x<imageReconstructed.height() && lp.first+x>=0 ){
                imageReconstructed(p.first+x,p.second+y)=inputImage(lp.first+x,lp.second+y);
                std::cout<<p.first+x<<"and"<<lp.first+x<<std::endl;

                }

            }
        }
    }

    }
    return imageReconstructed;
}






