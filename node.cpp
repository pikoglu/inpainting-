#include "node.h"

#include <numeric>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

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




    for (int y=patchsize/2;y<imageMask.height()-patchsize/2;y+=patchsize/2){
    for (int x=patchsize/2;x<imageMask.width()-patchsize/2;x+=patchsize/2){//attention aux nombres paires



             if (imageMask(x,y)>128){
                Point p(x,y);

                Node n(index,p,lmax);
                index++;
                //std::cout<<"index: "<<index<<std::endl;
                //std::cout<<"x: "<<x<<std::endl;
                //std::cout<<"y: "<<y<<std::endl;
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
    std::cout<<v[45].getx()<<" "<<v[45].gety()<<std::endl;
    std::cout<<v[45].getLeftNeighbor()<<std::endl;
    std::cout<<v[44].getTopNeighbor()<<std::endl;
    std::cout<<v[44].getBottomNeighbor()<<std::endl;
    std::cout<<v[44].getLeftNeighbor()<<std::endl;
    std::cout<<v[44].getRightNeighbor()<<std::endl;

    return  v;
}

bool sortArgbelief(Label a,Label b){
    return a.belief()>b.belief();
}

void Node::pushConditioned(const Label& label,int lmin,int lmax) {
    if (nodeConfusionSet.size()<lmin){
        nodeConfusionSet.push_back(label);
        std::sort(nodeConfusionSet.begin(),nodeConfusionSet.end(),sortArgbelief);

    }
    else{
        if (nodeConfusionSet.size()<lmax or label.belief()>nodeConfusionSet.back().belief()){
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

bool Node::thresholdConfusion(int thresholdConfusion,int lmin){

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

bool sortArgSize(Node a,Node b){
    if (a.size()==0) return false;
    if (b.size()==0) return true;
    return a.size()<b.size();}

std::vector<Node> assignInitialPriority( const Image& imageInput,const Image& imageMaskExtended,const Image& imageMask,
                                                       int patchSize, int lmin, int lmax,
                                                       int thresholdConfusion, int thresholdSimilarity) {


    std::vector<Node> confusionSets=nodesOverMask(imageMaskExtended,patchSize,lmax);



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





                if (imageMaskExtended(x,y)<128){ //black pixel ==> possible label candidate


                    Point labelPoint(x,y);
                    int potential=imageInput.ssdMask(nodePoint,labelPoint,imageMask,patchSize);
                    Label currentLabel(labelPoint,potential);



                    //If the size condition is met we start to discriminate the patches
                    //if (potential<thresholdConfusion){
                    if (confusionSets[i].size()<lmax or currentLabel.belief()>confusionSets[i].worstBelief()){ //suppose is is sorted
                        if ( confusionSets[i].similarityCondition(imageInput,labelPoint,thresholdSimilarity,patchSize)){
                            confusionSets[i].pushConditioned(currentLabel,lmin,lmax);
                        }
                    }

                }

            }

        }
        //we then apply the confusion threshold on the remaining confusionset

        confusionSets[i].thresholdConfusion(thresholdConfusion,lmin);
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

double Node::messageReceived(  const Node  &sender, const Point &coordPatchCandidate, const Image &imageInput,int patchSize){
    //correspond to the message reveived by this from sender by choosing coordPatchCandidate as patch
    double messageSent=0;
    double bestMessage=std::numeric_limits<double>::max();
    std::vector<Label> confusionSetSender=sender.getNodeConfusionSet();

    for (size_t i=0;i<confusionSetSender.size();i++){

        Label labelSender=confusionSetSender[i];
        messageSent=labelSender.getPotential();
        messageSent+=imageInput.ssdOverlap(sender.point(),(*this).point(),labelSender.point(),coordPatchCandidate,patchSize);
        assert(messageSent>=0);

        if (sender.point().first == patchSize/2+ (*this).point().first) { // Message to the left
            messageSent += labelSender.getMessageFromRight();
            messageSent += labelSender.getMessageFromTop();
            messageSent += labelSender.getMessageFromBottom();
        }
        else if (sender.point().first +patchSize/2== (*this).point().first) { // Message to the right
            messageSent += labelSender.getMessageFromLeft();
            messageSent += labelSender.getMessageFromTop();
            messageSent += labelSender.getMessageFromBottom();
        }
        else if (sender.point().second ==patchSize/2+ (*this).point().second) { // Message to the bottom
            messageSent += labelSender.getMessageFromLeft();
            messageSent += labelSender.getMessageFromRight();
            messageSent += labelSender.getMessageFromTop();
        }
        else if (sender.point().second +patchSize/2== (*this).point().second) { // Message to the top
            messageSent += labelSender.getMessageFromLeft();
            messageSent += labelSender.getMessageFromRight();
            messageSent += labelSender.getMessageFromBottom();
        }
        else{
            std::cout<<"message not from neigbhbor messageReceived"<<std::endl;
        }
        //std::cout<<i<<'/'<<messageSent<<std::endl;
        if (bestMessage>messageSent){
            bestMessage=messageSent;
        }

        //std::cout<<i<<'/'<<bestMessage<<std::endl;

    }

    return  bestMessage;
}

void Node::createNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
    if (sender.size()<lmin){
        std::cout<<"createNodeConfusionSet"<<std::endl;
        assert(sender.size()>=lmin);
    }
    assert((*this).getIndex()!=sender.getIndex());



    for (int x=patchSize/2;x<imageMaskExtended.width()-patchSize/2;x++) {
        for (int y=patchSize/2;y<imageMaskExtended.height()-patchSize/2;y++){

            if (imageMaskExtended(x,y)<128){ //black pixel ==> possible label candidate

                Point labelPoint(x,y);
                Label currentLabel(labelPoint,0);
                //std::cout<<"sender size :"<<sender.size()<<std::endl;
                int messageFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize);

                if (sender.point().first==patchSize/2+(*this).point().first){//Message from the right
                    //std::cout<<"avant :"<<currentLabel.getMessageFromRight()<<std::endl;

                    //std::cout<<messageFromSender<<std::endl;

                    currentLabel.setMessageFromRight(messageFromSender);//pos potential left right top botoom
                    //std::cout<<"apres :"<<currentLabel.getMessageFromRight()<<std::endl;
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


                //If the size condition is met we start to discriminate the patches
                //if (potential<thresholdConfusion){
                if ((*this).size()<lmax or currentLabel.belief()>(*this).worstBelief()){ //suppose is is sorted
                    if ( (*this).similarityCondition(imageInput,labelPoint,thresholdSimilarity,patchSize)){
                        (*this).pushConditioned(currentLabel,lmin,lmax);
                    }
                }

            }

        }

    }

    if ((*this).size()<lmin){
        std::cout<<"createNodeConfusionSet"<<std::endl;
        assert((*this).size()>=lmin);
    }

    (*this).thresholdConfusion(thresholdConfusion,lmin);
    /*
    for (size_t k=0;k<(*this).size();k++){
        std::cout<<k<<std::endl;
        std::cout<<"x :"<<label(k).getx()<<std::endl;
        std::cout<<"y :"<<label(k).gety()<<std::endl;
        std::cout<<"top : "<<(*this).label(k).getMessageFromTop()<<std::endl;
        std::cout<<"bottom :"<<(*this).label(k).getMessageFromBottom()<<std::endl;
        std::cout<<"left : "<<(*this).label(k).getMessageFromRight()<<std::endl;
        std::cout<<"right :"<<(*this).label(k).getMessageFromLeft()<<std::endl;
        std::cout<<std::endl;
    }*/

}

void Node::updateNodeConfusionSet(const Node &sender, const Image &imageMaskExtended,const Image &imageInput,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
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
    std::vector<Label> & receiverNodeConfusionSet=this->getNodeConfusionSet();

    for (size_t i=0;i<(*this).size();i++) {

            Point labelPoint(receiverNodeConfusionSet[i].point());
            int messageFromSender=(*this).messageReceived(sender,labelPoint,imageInput,patchSize);
            if (sender.point().first>(*this).point().first){//Message from the right
                //std::cout<<"avant"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
                receiverNodeConfusionSet[i].setMessageFromRight(messageFromSender);//pos potential left right top botom
                //std::cout<<"apres"<<this->getNodeConfusionSet()[i].getMessageFromRight()<<std::endl;
            }
            else if(sender.point().first<(*this).point().first){//Message from the left
                receiverNodeConfusionSet[i].setMessageFromLeft(messageFromSender);
            }
            else if(sender.point().second>(*this).point().second){//Message from the top
                receiverNodeConfusionSet[i].setMessageFromTop(messageFromSender);
            }
            else if (sender.point().second<(*this).point().second) {//Message from the bottom
                receiverNodeConfusionSet[i].setMessageFromBottom(messageFromSender);
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

size_t getNodeOfIndex(const std::vector<Node>& InitialPriority, int j) {
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

Image forwardPass(std::vector<Node> &InitialPriority,const Image &imageInput, const Image &imageMaskExtended,int patchSize,int thresholdSimilarity,int thresholdConfusion,int lmin,int lmax){
    std::vector<int> commitStack;
    std::vector<bool> commitList=intializeFalseVector(InitialPriority.size());
    Image orderOfVisit=imageMaskExtended.clone();
    size_t indexCommitSearch;
    int progression=0;
    //std::cout<<"noeud du bas : "<<getNodeOfCoord(InitialPriority,289,73)<<std::endl;
    //std::cout<<"noeud du haut : "<<getNodeOfCoord(InitialPriority,289,49)<<std::endl;
    //std::cout<<"noeud du gauche : "<<getNodeOfCoord(InitialPriority,277,61)<<std::endl;
    //std::cout<<"noeud du droite : "<<getNodeOfCoord(InitialPriority,301,61)<<std::endl;
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


        /*
        Node NoeudSix=InitialPriority[getNodeOfIndex(InitialPriority,6)];
        std::cout<<"x : "<<NoeudSix.getx()<<std::endl;
        std::cout<<"y :"<<NoeudSix.gety()<<std::endl;
        std::cout<<"index :"<<NoeudSix.getIndex()<<std::endl;
        for (size_t k=0;k<NoeudSix.size();k++){
            std::cout<<k<<std::endl;
            std::cout<<"top : "<<NoeudSix.label(k).getMessageFromTop()<<std::endl;
            std::cout<<"bottom :"<<NoeudSix.label(k).getMessageFromBottom()<<std::endl;
            std::cout<<"left : "<<NoeudSix.label(k).getMessageFromRight()<<std::endl;
            std::cout<<"right :"<<NoeudSix.label(k).getMessageFromLeft()<<std::endl;
            std::cout<<std::endl;
        }*/


        if (time==InitialPriority.size()-1){
            std::cout<<"x : "<<currentNode.getx()<<std::endl;
            std::cout<<"y :"<<currentNode.gety()<<std::endl;
            std::cout<<"index :"<<currentNode.getIndex()<<std::endl;
            for (size_t k=0;k<currentNode.size();k++){
                std::cout<<k<<std::endl;
                std::cout<<"top : "<<currentNode.label(k).getMessageFromTop()<<std::endl;
                std::cout<<"bottom :"<<currentNode.label(k).getMessageFromBottom()<<std::endl;
                std::cout<<"left : "<<currentNode.label(k).getMessageFromRight()<<std::endl;
                std::cout<<"right :"<<currentNode.label(k).getMessageFromLeft()<<std::endl;
                std::cout<<std::endl;
            }
        }
        commitStack.push_back(currentNode.getIndex());
        commitList[InitialPriority[indexCommitSearch].getIndex()]=true;


        //Temporaire
        for (int x=-patchSize/2;x<=patchSize/2;x++){
            for (int y=-patchSize/2;y<=patchSize/2;y++){
                orderOfVisit(currentNode.getx() +x,currentNode.gety()+y,0)=int((float(time)-1)/float(InitialPriority.size())*255);

            }

        }

        //fin temporaire


        if (currentNode.hasLeftNeighbor() && not commitList[currentNode.getLeftNeighbor()]) {

            int indexLeftNeighbor = getNodeOfIndex(InitialPriority, currentNode.getLeftNeighbor());


            if (indexLeftNeighbor==6) std::cout<<"voisin de droite :"<<currentNode.getIndex()<<std::endl;
            if (InitialPriority[indexLeftNeighbor].size() == 0) { // interior
                InitialPriority[indexLeftNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                InitialPriority[indexLeftNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasRightNeighbor() && not commitList[currentNode.getRightNeighbor()]) {
            int indexRightNeighbor = getNodeOfIndex(InitialPriority, currentNode.getRightNeighbor());
            if (indexRightNeighbor==6) std::cout<<"voisin de gauche :"<<currentNode.getIndex()<<std::endl;
            if (InitialPriority[indexRightNeighbor].size() == 0) { // interior
                InitialPriority[indexRightNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                InitialPriority[indexRightNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasTopNeighbor() && not commitList[currentNode.getTopNeighbor()]) {
            int indexTopNeighbor = getNodeOfIndex(InitialPriority, currentNode.getTopNeighbor());
            if (indexTopNeighbor==6) std::cout<<"voisin du bas  :"<<currentNode.getIndex()<<std::endl;

            if (InitialPriority[indexTopNeighbor].size() == 0) { // interior
                InitialPriority[indexTopNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                InitialPriority[indexTopNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }

        if (currentNode.hasBottomNeighbor() && not commitList[currentNode.getBottomNeighbor()]) {
            int indexBottomNeighbor = getNodeOfIndex(InitialPriority, currentNode.getBottomNeighbor());
            if (indexBottomNeighbor==6) std::cout<<"voisin du haut  :"<<currentNode.getIndex()<<std::endl;

            if (InitialPriority[indexBottomNeighbor].size() == 0) { // interior
                InitialPriority[indexBottomNeighbor].createNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
            else {
                InitialPriority[indexBottomNeighbor].updateNodeConfusionSet(currentNode, imageMaskExtended, imageInput, patchSize, thresholdSimilarity, thresholdConfusion, lmin, lmax);
            }
        }
        std::sort(InitialPriority.begin(),InitialPriority.end(),sortArgSize);
    }
    return orderOfVisit;
}

Image imageReconstructed(const std::vector<Node> &InitialPriority, int patchSize,Image inputImage,Image maskImage){

    Image imageReconstructed=inputImage.clone();
    for (size_t i = InitialPriority.size(); i-- > 0; ) {//cor
        if (true){//InitialPriority[i].size()>1){
        Point p=InitialPriority[i].point();

        Label l=InitialPriority[i].label(0);
        Point lp=l.point();
            for (int x=-patchSize/2;x<=patchSize/2;x++){
                for (int y=-patchSize/2;y<=patchSize/2;y++){
                    imageReconstructed(p.first+x,p.second +y,0)=inputImage(lp.first+x,lp.second+y,0);
                    imageReconstructed(p.first+x,p.second+y,1)=inputImage(lp.first+x,lp.second+y,1);
                    imageReconstructed(p.first+x,p.second+y,2)=inputImage(lp.first+x,lp.second+y,2);

                    imageReconstructed(lp.first+x,lp.second+y,0)=255;




            }
        }
    }

    }
    return imageReconstructed;
}






