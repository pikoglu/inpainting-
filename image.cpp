/**
 * @file image.cpp
 * @brief image class with shallow copy
 * @author Pascal Monasse <monasse@imagine.enpc.fr>
 *
 * Copyright (c) 2012-2014, Pascal Monasse
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Pulic License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "image.h"
#include "io_png.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>


/// Constructor
///
/// The main interest of this one is to allow arrays of Image.
Image::Image()
: count(0), tab(0), w(0), h(0), c(0) {}

/// Constructor
Image::Image(int width, int height, int channels)
: count(new int(1)), tab(new float[width*height*channels]),
  w(width), h(height), c(channels) {}

/// Constructor with array of pixels.
///
/// Make sure it is not deleted during the lifetime of the image.
Image::Image(float* pix, int width, int height, int channels)
: count(0), tab(pix), w(width), h(height), c(channels) {}

/// Copy constructor (shallow copy)
Image::Image(const Image& I)
: count(I.count), tab(I.tab), w(I.w), h(I.h), c(I.c) {
    if(count)
        ++*count;
}

/// Assignment operator (shallow copy)
Image& Image::operator=(const Image& I) {
    if(count != I.count) {
        kill();
        if(I.count)
            ++*I.count;
    }
    count=I.count; tab=I.tab; w=I.w; h=I.h; c=I.c;
    return *this;
}

/// Deep copy
Image Image::clone() const {
    Image I(w,h,c);
    std::copy(tab, tab+w*h*c, I.tab);
    return I;
}

/// Free memory
void Image::kill() {
    if(count && --*count == 0) {
        delete count;
        delete [] tab;
    }
}

/// Convert image to gray level
Image Image::gray() const {
    if(channels() == 1)
        return *this;
    assert(channels() == 3);
    Image out(w,h);
    const float* in = tab;
    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++, in+=3)
            out(x,y) = rgb_to_gray(in[0], in[1], in[2]);
    return out;
}



//Felix


int* Image::listPatchCenters(int patchSize) const {
    int numCenters = (w - patchSize + 1) * (h - patchSize + 1);
    int* centers = new int[numCenters * 2]; // Each patch center has x and y coordinates

    int halfPatchSize = patchSize / 2;
    int index = 0;

    for (int y = halfPatchSize; y < h - halfPatchSize; y++) {
        for (int x = halfPatchSize; x < w - halfPatchSize; x++) {
            centers[index++] = x;
            centers[index++] = y;
        }
    }

    return centers;
}

int Image::ssd(int xp1, int yp1, int xp2, int yp2, Image const &mask,int patch_size) const{
    int half_patch = patch_size / 2;
    int sum = 0;

    for (int y = -half_patch; y <= half_patch; y++) {
        for (int x = -half_patch; x <= half_patch; x++) {
            for (int channel = 0; channel < c; channel++) {
                if (mask(xp1 + x,yp1 + y)<128){
                    int diff = (*this)(xp1 + x, yp1 + y, channel) - (*this)(xp2 + x, yp2 + y, channel);
                    sum += diff * diff;}
            }
        }
    }

    return sum;
}



Image Image::createSSDImage(int patchSize, int ip) const {
    int width = this->width();
    int height = this->height();
    Image grayImage(width, height, 1); // c=1 (gris)

    int numpatchs = (width - patchSize + 1) * (height - patchSize + 1);
    int* centers = this->listPatchCenters(patchSize);

    int xp = centers[2 * ip];
    int yp = centers[2 * ip + 1];

    float ssdMax = float(0);

    for (int j = 0; j < numpatchs; ++j) {
        int x2 = centers[2 * j];
        int y2 = centers[2 * j + 1];
        int ssd_patch = this->ssd(xp, yp, x2, y2, (*this),patchSize);
        ssdMax = std::max(ssdMax, static_cast<float>(ssd_patch)); //Attention il est sans doute nécessaire de
    }

    //etape de normalisation
    for (int j = 0; j < numpatchs; ++j) {
        int x2 = centers[2 * j];
        int y2 = centers[2 * j + 1];
        int ssd_patch = this->ssd(xp, yp, x2, y2,(*this), patchSize);
        float ssdNormalized = static_cast<float>(ssd_patch) / ssdMax;
        if (xp==x2 and yp==y2){
            grayImage(x2, y2) = 255;
            std::cout<<ssdNormalized * 255.0f<<std::endl;
            std::cout<<x2<<" "<<y2<<std::endl;} //noir proche, blanc loin
        else{
        grayImage(x2, y2) = ssdNormalized * 255.0f;}
    }

    delete[] centers;

    return grayImage;
}


/*
bool save_image(const char* fileName, const Image& img) {
    const int w = img.width();
    const int h = img.height();
    const int c = img.channels();

    float* out = new float[w * h * c];
    float* o = out;

    for (int channel = 0; channel < c; ++channel) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                *o++ = img(x, y, channel);
            }
        }
    }

    bool ok = (io_png_write_f32(fileName, out, w, h, c) == 0);

    delete[] out;
    return ok;
}
*/



bool save_image(const char* fileName, const Image& img) {
    const int w = img.width();
    const int h = img.height();
    const int c = img.channels();

    // Changer le type de float* à unsigned char*
    unsigned char* out = new unsigned char[w * h * c];
    unsigned char* o = out;

    for (int channel = 0; channel < c; ++channel) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                unsigned char value = static_cast<unsigned char> (img(x, y, channel));
                *o++ = value;
            }
        }
    }

    bool ok = (io_png_write_u8(fileName, out, w, h, c) == 0);

    delete[] out;
    return ok;
}


int Image::getPatchIndexFromCoordinates(int x, int y, int patchSize) const {
    int width = this->width();
    int height = this->height();
    int numPatches = (width - patchSize + 1) * (height - patchSize + 1);
    int halfPatchSize = patchSize / 2;

    int* centers = this->listPatchCenters(patchSize);

    for (int i = 0; i < numPatches; ++i) {
        int centerX = centers[i * 2];
        int centerY = centers[i * 2 + 1];

        if (x ==centerX  && y ==centerY ) {
            delete[] centers;
            return i;
        }
    }

    delete[] centers;
    return -1; // Pixel not found in any patch
}

void Image::initializeToBool(){
    assert(channels()==1);
    for (int y=0;y<height();y++){
        for (int x=0;x<width();x++){
            (*this)(y,x)=0.0f;
        }
    }
}



Image Image::extendMask(int patchsize)const { // a changer c'est patchsize/2
    assert(channels() == 1);
    int width = this->width();
    int height = this->height();


    Image extendedMask=(*this).clone();
    extendedMask.gray();

    // Parcours des lignes

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if ((*this)(x, y) > 128) {
                for (int i = std::max(0, x - patchsize/2); i < x; ++i) {
                    extendedMask(i, y) = 255;
                }
                x++;
                while(x<width && (*this)(x, y) > 128){x++;}

                if (x<width){
                    for (int i = std::min(width-1, x + patchsize/2); i >= x; --i) {//attention x n'est pas dans le masque
                        extendedMask(i, y) = 255;
                    }
                }

            }
        }
    }

    Image extendedMaskTemp=extendedMask.clone();

    for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if ((extendedMaskTemp)(x, y) > 128) {
                    for (int i = std::max(0, y - patchsize/2 ); i < y; ++i) {
                        extendedMask(x, i) = 255;}
                    y++;
                    while (y < height && (extendedMaskTemp)(x, y) > 128) {
                        y++;
                    }
                    if (y < height) {
                        for (int i = y; i < std::min(height, y + patchsize/2); ++i) {
                            extendedMask(x, i) = 255;
                    }
                }
            }
        }
    }

    return extendedMask;

}


std::vector<Node> Image::nodesOverMask(int patchsize) const{


    //Ne fonctionne que dans le cas impaire
    std::vector<Node> v;
    int index=0;

    int numberUniquePatchWidth = (width()-patchsize)/((patchsize-1)/2)+1;
    int numberUniquePatchHeight = (height()-patchsize)/((patchsize-1)/2)+1;

    int* listNodes=new int[2*numberUniquePatchWidth*numberUniquePatchHeight];
    int halfPatchSize = patchsize / 2;

    for (int y=patchsize/2+1;y<height()-patchsize/2;y+=patchsize/2){
        for (int x=patchsize/2+1;x<width()-patchsize/2;x+=patchsize/2){//attention aux nombres paires



             if ((*this)(x,y)>128){

                Node n(index,x,y);
                v.push_back(n);

                //is there an already existing neighbors at the left ???

                if (1<v.size()){
                    if (v[v.size()-2].getx()==x-patchsize/2 && v[v.size()-2].gety()==y){
                        v.back().addLeftNeighbor(v.size()-2);
                        v[v.size()-2].addRightNeighbor(v.size()-1);


                    }
                }
                //is there an already existing neighbors at the top ???

                for (size_t i=0; i<v.size();i++){
                    if (v[i].getx()==x && v[i].gety()==y-patchsize/2){
                        if (y==39){std::cout<<"attention"<<std::endl;}
                        v.back().addTopNeighbor(i);
                        v[i].addBottomNeighbor(v.size()-1);
                        break;

                    }
                }
             }
         }
    }
    return  v;
}



void Image::displayNodesOverMask(int* listNodes, int patchSize){
    std::cout<<channels()<<std::endl;
    int numberUniquePatchWidth = (2 * width()) / patchSize - 1;
    int numberUniquePatchHeight = (2 * height()) / patchSize - 1;


    int x;
    int y;

    for (int index=0;index<numberUniquePatchWidth*numberUniquePatchHeight;index+=2){
        x=listNodes[index];
        y=listNodes[index+1];
        std::cout<<index<<std::endl;
        std::cout<<x<<std::endl;
        std::cout<<y<<std::endl;
        (*this)(x,y,0)=255;
        (*this)(x,y,1)=0;
        (*this)(x,y,2)=0;
    }
}


void Image::visualiseNodesAndVertices(std::vector<Node> v,int patchsize) {


    for (size_t i=0; i<v.size();i++){
        Node n=v[i];
        //std::cout<<n.getx()<<" "<<n.gety()<<std::endl;
        (*this)(n.getx(),n.gety(),0)=255;
        (*this)(n.getx(),n.gety(),1)=0;
        (*this)(n.getx(),n.gety(),2)=0;



        if ( n.getTopNeighbor()!=-1){
            for (int j=1;j<patchsize/2;j++){
                (*this)(n.getx(),n.gety()-j,1)=255;
                (*this)(n.getx(),n.gety()-j,0)=0;
                (*this)(n.getx(),n.gety()-j,2)=0;
            }
        }



        // Visualize bottom neighbor vertices as green
        if (n.getBottomNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                (*this)(n.getx() , n.gety()+j, 0) = 0;
                (*this)(n.getx() , n.gety()+j, 1) = 255;
                (*this)(n.getx() , n.gety()+j, 2) = 0;
            }
        }

        // Visualize left neighbor vertices as green
        if (n.getLeftNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                (*this)(n.getx()- j, n.gety() , 0) = 0;
                (*this)(n.getx()- j, n.gety() , 1) = 255;
                (*this)(n.getx()- j, n.gety() , 2) = 0;
            }
        }
        // Visualize right neighbor vertices as green
        if (n.getRightNeighbor() != -1) {
            for (int j = 1; j < patchsize / 2; j++) {
                (*this)(n.getx()+ j, n.gety() , 0) = 0;
                (*this)(n.getx()+ j, n.gety() , 1) = 255;
                (*this)(n.getx()+ j, n.gety(), 2) = 0;
            }
        }
    }
}

bool Image::isPatchInsideMask(int xp,int yp , int patchSize) const{
    //to be called on mask
    for (int x=xp-patchSize/2;x<=xp+patchSize/2;x++){
        for (int y=yp-patchSize/2;y<=yp+patchSize/2;y++){
            if ((*this)(x,y)<128){return false;} //black pixels ==> patch not entirely inside mask
        }
    }
    return true;


}

//ajouter tyedef point pour std::pair<int,int>
//définir d'autre typedef
//soigner le code

bool compareBeliefByPotential(const Belief& a, const Belief& b) { //pour trier ConfusionSet par potentiel croissant
    return a.second < b.second;
}

std::vector<ConfusionSet> Image::assignInitialPriority(const std::vector<Node>& nodes, const Image& maskExtended,const Image& mask,
                                                       int patchSize, int Lmin, int Lmax,
                                                       int thresholdConfusion, int thresholdSimilarity)const{
    std::vector<ConfusionSet > confusionSets;
    confusionSets.reserve(nodes.size());//size is known

    int progression=0;

    // Iterate over each node
    for (size_t i=0;i<nodes.size();i++) {if (i/100>progression){progression=i/100;std::cout<<progression*100<<"/"<<nodes.size()<<std::endl;}

        int nodeX = nodes[i].getx();
        int nodeY = nodes[i].gety();


        // If patch is entirely inside the mask the initial discimination is impossible therefore we let the confusion set empty (<==> confusion sets are all the labels)
        if (mask.isPatchInsideMask(nodeX,nodeY,patchSize)) {//optimisation possible since
            ConfusionSet nodeConfusionSet;//empty
            confusionSets.push_back(nodeConfusionSet);
            continue;
        }

        // Compute SSD with each label patch in maskExtended
        ConfusionSet nodeConfusionSet;

        for (int x=patchSize/2;x<maskExtended.width()-patchSize/2;x++) {
            for (int y=patchSize/2;y<maskExtended.height()-patchSize/2;y++){
                if (maskExtended(x,y)<128){ //black pixel ==> possible label candidate
                    int potential=ssd(nodeX,nodeY,x,y,mask,patchSize);

                    //If there is not enough labels already to begin discrimination
                    if (nodeConfusionSet.size()<Lmin){ //if the minimum hasn't been met
                        Point coordinateLabel(x,y);
                        Belief initialBelief(coordinateLabel,potential);
                        nodeConfusionSet.push_back(initialBelief);
                        if (nodeConfusionSet.size()==Lmin){
                            std::sort(nodeConfusionSet.begin(),nodeConfusionSet.end(),compareBeliefByPotential); //sorted increasing order
                        }
                        continue;

                    }

                    //If the size condition is met we start to discriminate the patches
                    //if (potential<thresholdConfusion){
                    if (nodeConfusionSet.size()<Lmax or potential<nodeConfusionSet.front().first.second){ //suppose is is sorted

                        //Does this patch respect the patch similarity condition
                        bool breaked=false;
                        for (size_t i=0;i<nodeConfusionSet.size();i++){
                            Point coordinate=nodeConfusionSet[i].first;
                            int xLabel=coordinate.first;
                            int yLabel=coordinate.second;
                            int ssdDistance= ssd(x,y,xLabel,yLabel,mask,patchSize);
                            if (ssdDistance<thresholdSimilarity) {//no need to normalize since patch are full
                                breaked=true;
                                break; // we found one to similar no add

                            }
                        }

                        //Is it hasn't break it should be added and inserted in the good position to conserve sort
                        if (not breaked){
                            for (size_t j=1;j<nodeConfusionSet.size();j++){
                                //we insert the new label
                                if (nodeConfusionSet.size()==Lmax){
                                    nodeConfusionSet.pop_back();
                                }
                                if (potential>=nodeConfusionSet[j].first.second){
                                    Point newCoordonates(x,y);
                                    Belief newNode(newCoordonates,potential);
                                    nodeConfusionSet.insert(nodeConfusionSet.begin()+j,newNode);

                                }
                            }
                        }
                    }

                }

            }

        }
        if (nodeConfusionSet.size() > Lmin) {
            std::vector<Belief>::iterator it = std::lower_bound(nodeConfusionSet.begin(), nodeConfusionSet.end(),
                                       Belief(Point(0,0), thresholdConfusion-nodeConfusionSet[0].second),
                                       compareBeliefByPotential);




            // Keep at least
            if (std::distance(nodeConfusionSet.begin(), it) < Lmin) {
                it = nodeConfusionSet.begin() + Lmin;
            }

            while (nodeConfusionSet.size() > std::distance(nodeConfusionSet.begin(), it)) {
                nodeConfusionSet.pop_back();
            }

        }

        confusionSets.push_back(nodeConfusionSet);


    }



    return confusionSets;
}


/*
std::vector<ConfusionSet> Image::assignInitialPriority(const std::vector<Node>& nodes, const Image& maskExtended,
                                                       int patchSize, int Lmin, int Lmax,
                                                       int thresholdConfusion, int thresholdSimilarity)const {
    std::vector<ConfusionSet> confusionSets;
    confusionSets.reserve(nodes.size());

    for (size_t i=0;i<nodes.size();i++) {
        std::cout<<i<<"/"<<nodes.size()<<std::endl;
        int nodeX = nodes[i].getx();
        int nodeY = nodes[i].gety();

        if (maskExtended.isPatchInsideMask(nodeX, nodeY, patchSize)) {
            continue;
        }


        std::priority_queue<Belief, std::vector<Belief>, std::greater<Belief> > nodeConfusionSet;

        for (int x = patchSize/2; x < maskExtended.width() - patchSize/2; ++x) {
            for (int y = patchSize/2; y < maskExtended.height() - patchSize/2; ++y) {
                if (maskExtended(x, y) >= 128) continue; // Skip non-black pixels

                int potential = ssd(nodeX, nodeY, x, y, patchSize);

                if (nodeConfusionSet.size() < Lmin) {
                    nodeConfusionSet.push(Belief(Point(x, y), potential));
                    continue;
                }

                if (potential < thresholdConfusion &&
                    (nodeConfusionSet.size() < Lmax || potential < nodeConfusionSet.top().second)) {

                    bool isSimilar = false;
                    std::vector<Belief> temp;
                    temp.reserve(nodeConfusionSet.size());

                    while (!nodeConfusionSet.empty()) {
                        const auto& [coord, _] = nodeConfusionSet.top();
                        int ssdDistance = ssd(x, y, coord.first, coord.second, patchSize);
                        if (ssdDistance < thresholdSimilarity) {
                            isSimilar = true;
                            break;
                        }
                        temp.push_back(nodeConfusionSet.top());
                        nodeConfusionSet.pop();
                    }

                    if (!isSimilar) {
                        nodeConfusionSet.push(Belief(Point(x, y), potential));
                        if (nodeConfusionSet.size() > Lmax) {
                            nodeConfusionSet.pop();
                        }
                    }

                    for (const auto& item : temp) {
                        nodeConfusionSet.push(item);
                    }
                }
            }
        }

        ConfusionSet finalSet;
        finalSet.reserve(nodeConfusionSet.size());
        while (!nodeConfusionSet.empty()) {
            finalSet.push_back(nodeConfusionSet.top());
            nodeConfusionSet.pop();
        }

        confusionSets.push_back(std::move(finalSet));
    }

    return confusionSets;
}*/
