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
#include <cstring>


/// Constructor
///
/// The main interest of this one is to allow arrays of Image.
Image::Image()
    : count(0), tab(0), w(0), h(0), c(0) {}

Image::Image(int width, int height, int channels)
    : count(new int(1)), tab(new int[width*height*channels]),
    w(width), h(height), c(channels) {}


/// Constructor with array of pixels.
///
/// Make sure it is not deleted during the lifetime of the image.
Image::Image(int* pix, int width, int height, int channels)
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
    const int* in = tab;
    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++, in+=3)
            out(x,y,0) = rgb_to_gray(in[0], in[1], in[2]);
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

int Image::ssdMask(Point point1, Point point2, Image const &mask,int patch_size) const{
    int half_patch = patch_size / 2;
    int sum = 0;

    for (int y = -half_patch; y <= half_patch; y++) {
        if (y>=0 && y<mask.height()){
            for (int x = -half_patch; x <= half_patch; x++) {
                if (x>=0 && x<mask.width()){
                    if (mask(point1.first + x,point1.second + y,0)<128){
                        for (int channel = 0; channel < c; channel++) {
                            int diff = (*this)(point1.first + x, point1.second + y, channel)
                                        - (*this)(point2.first + x, point2.second + y, channel);
                            sum += diff * diff;
                        }
                    }
                }
            }
        }
    }
    return sum;
}


int Image::ssd(Point point1, Point point2,int patch_size) const{
    int half_patch = patch_size / 2;
    int sum = 0;

    for (int y = -half_patch; y <= half_patch; y++) {
        if (y>=0 && y<height()){
            for (int x = -half_patch; x <= half_patch; x++) {
                if (x>=0 && x<width()){
                    for (int channel = 0; channel < c; channel++) {
                        int diff = (*this)(point1.first + x, point1.second + y, channel) - (*this)(point2.first + x, point2.second + y, channel);
                        sum += diff * diff;
                    }
                }
            }

        }
    }
    return sum;
}





bool save_image(const char* fileName, const Image& img) {
    const int w = img.width();
    const int h = img.height();
    const int c = img.channels();
    std::cout<<"save 1"<<std::endl;
    std::cout<<fileName<<std::endl;
    std::cout<<c<<std::endl;




    // Changer le type de float* à unsigned char*
    unsigned char* out = new unsigned char[w * h * c];
    unsigned char* o = out;

    for (int channel = 0; channel < c; ++channel) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (  std::string(fileName)=="/Users/felixfourreau/Desktop/ProjetStageCourt/data/candidates/1R_4S_48I_240X_135Y_41M.png"
                    ||std::string(fileName)=="/Users/felixfourreau/Desktop/ProjetStageCourt/data/candidates/1R_4S_42I_255X_120Y_41M.png"
                    ||std::string(fileName)=="/Users/felixfourreau/Desktop/ProjetStageCourt/data/candidates/1R_0S_33I_225X_105Y_32M.png"
                    ||std::string(fileName)=="/Users/felixfourreau/Desktop/ProjetStageCourt/data/candidates/1R_4S_42I_255X_120Y_41M.png"){
                    std::cout<< img(x, y, channel)<<std::endl;
                }


                unsigned char value = static_cast<unsigned char> (img(x, y, channel));

                *o++ = value;
            }
        }
    }

    std::cout<<"save 2"<<std::endl;

    bool ok = (io_png_write_u8(fileName, out, w, h, c) == 0);
    std::cout<<"save 3"<<std::endl;

    delete[] out;
    return ok;
}


int Image::getPatchIndexFromCoordinates(int x, int y, int patchSize) const {
    int width = this->width();
    int height = this->height();
    int numPatches = (width - patchSize + 1) * (height - patchSize + 1);

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
            (*this)(y,x,0)=0.0f;
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
            if ((*this)(x, y,0) > 128) {
                for (int i = std::max(0, x - patchsize/2); i < x; ++i) {
                    extendedMask(i, y,0) = 255;
                }
                x++;
                while(x<width && (*this)(x, y,0) > 128){x++;}

                if (x<width){
                    for (int i = x; i<std::min(width, x + patchsize/2); ++i) {//attention x n'est pas dans le masque
                        extendedMask(i, y,0) = 255;
                    }
                }

            }
        }
    }

    Image extendedMaskTemp=extendedMask.clone();

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if ((extendedMaskTemp)(x, y,0) > 128) {
                for (int i = std::max(0, y - patchsize/2 ); i < y; ++i) {
                    extendedMask(x, i,0) = 255;
                }
                y++;
                while (y < height && (extendedMaskTemp)(x, y,0) > 128) {
                    y++;
                }
                if (y < height) {
                    for (int i = y; i < std::min(height, y + patchsize/2); ++i) {
                        extendedMask(x, i,0) = 255;
                    }
                }
            }
        }
    }

    return extendedMask;

}










bool Image::isPatchInsideMask(Point patchPoint , int patchSize) const{
    //to be called on mask
    /*
    for (int x=patchPoint.first-patchSize/2;x<=patchPoint.first+patchSize/2;x++){
        for (int y=patchPoint.second-patchSize/2;y<=patchPoint.second+patchSize/2;y++){
            if ((*this)(x,y,0)<128){return false;} //black pixels ==> patch not entirely inside mask
        }
    }*/

    if ((*this)(patchPoint.first,patchPoint.second,0)<128){return false;}
    return true;


}

//ajouter tyedef point pour std::pair<int,int>
//définir d'autre typedef
//soigner le code

bool compareBeliefByPotential(const Belief& a, const Belief& b) { //pour trier ConfusionSet par potentiel croissant

    int beliefa=0;
    int beliefb=0;

    for (int i=0;i<5;i++){
        beliefa+=a.second[i];
        beliefb+=b.second[i];
    }
    return beliefa < beliefb;
}


void printVector(ConfusionSet const nodeConfusionSet){
    for (size_t i=0;i<nodeConfusionSet.size();i++){
        std::cout<<nodeConfusionSet[i].second[0]<<std::endl;
    }

}






double Image::ssdOverlap(Point n1, Point n2, Point p1, Point p2, int patchSize,int w0) const {
    double ssd = 0;

    for (int j=0;j<c;j++){
        if (n1.first +patchSize/2==n2.first) { //node 1 on the left
            for (int x = -patchSize/2; x <= 0; x++) {
                for (int y = -patchSize/2; y <= patchSize/2; y++) {
                    double diff = (*this)(p1.first +patchSize/2 + x, p1.second + y,j) - (*this)(p2.first + x, p2.second + y,j);
                    ssd += diff * diff;
                }
            }
        } else if (n2.first +patchSize/2 == n1.first) {//node 2 on the left
            for (int x = -patchSize/2; x <= 0; x++) {
                for (int y = -patchSize/2; y <= patchSize/2; y++) {
                    double diff = (*this)(p1.first + x, p1.second + y,j) - (*this)(p2.first +patchSize/2 + x, p2.second + y,j);
                    ssd += diff * diff;


                }
            }
        } else if (n1.second+patchSize/2 == n2.second) {//n1 on the bottom
            for (int x = -patchSize/2; x <= patchSize/2; x++) {
                for (int y = 0; y < patchSize/2 + 1; y++) {
                    double diff = (*this)(p1.first + x, p1.second + y,j) - (*this)(p2.first + x, p2.second - patchSize/2 + y,j);
                    ssd += diff * diff;
                }
            }
        } else if (n2.second+patchSize/2 == n1.second){//n2 on the bottom
            for (int x = -patchSize/2; x <= patchSize/2; x++) {
                for (int y = -patchSize/2; y <= 0; y++) {
                    double diff = (*this)(p1.first + x, p1.second + y,j) - (*this)(p2.first + x, p2.second + patchSize/2 + y,j);
                    ssd += diff * diff;
                }
            }
        }
        else{
            std::cout<<"error std overlap"<<std::endl;
        }
    }

    if (std::abs(p1.first-p2.first)-patchSize>0 and std::abs(p1.second-p2.second)-patchSize>0){
        ssd=ssd+w0;
    }


    return ssd;
}


Image Image::simplifyMaskToOnePixel(int x1, int y1, int x2, int y2)const{
    assert(x1<(*this).width() && x2<(*this).width() && x1>=0 && x2>=0 );
    assert(y1<(*this).height() && y2<(*this).height() && y1>=0 && y2>=0 );
    assert(x1<x2);
    assert(y1<y2);

    Image simplifiedMask( (*this).width(),(*this).height(),1);

    for (size_t x=0;x<(*this).width();x++){
        for (size_t y=0;y<(*this).height();y++){
            simplifiedMask(x,y,0)=0;

        }
    }

    for (size_t xp=x1;xp<x2;xp++){
        for (size_t yp=y1;yp<y2;yp++){
            simplifiedMask(xp,yp,0)=255;

        }
    }
    return simplifiedMask;
}


double Image::ssdOverlap_fft(Point n1, Point n2, Point p1, Point p2, int patchSize, int w0) const {
    double ssd = 0;

    // Determine the overlap type
    int overlapType = getOverlapType(n1, n2, patchSize);

    // Extract overlapping regions based on overlap type
    std::vector<std::complex<float> > patch1, patch2;
    int overlapWidth, overlapHeight;

    switch(overlapType) {
    case 1: // node 1 on the left
    case 2: // node 2 on the left
        overlapWidth = patchSize / 2 + 1;
        overlapHeight = patchSize;
        extractOverlapPatches(p1, p2, patchSize, overlapWidth, overlapHeight, overlapType, patch1, patch2);
        break;
    case 3: // n1 on the bottom
    case 4: // n2 on the bottom
        overlapWidth = patchSize;
        overlapHeight = patchSize / 2 + 1;
        extractOverlapPatches(p1, p2, patchSize, overlapWidth, overlapHeight, overlapType, patch1, patch2);
        break;
    default:
        throw std::runtime_error("Invalid overlap type");
    }

    ssd = computeFFTBasedSSD(patch1, patch2, overlapWidth, overlapHeight);

    if (std::abs(p1.first - p2.first) - patchSize > 0 && std::abs(p1.second - p2.second) - patchSize > 0) {
        ssd += w0;
    }

    return patchSize*patchSize*255*255*3- ssd;
}


int Image::ssd_fft(Point point1, Point point2, int patch_size) const {
    std::vector<std::complex<float> > patch1 = extractPatch(point1, patch_size);
    std::vector<std::complex<float> > patch2 = extractPatch(point2, patch_size);

    int padded_size = nextPowerOf2(patch_size);
    std::vector<std::complex<float> > padded1 = padPatch(patch1, patch_size, padded_size);
    std::vector<std::complex<float> > padded2 = padPatch(patch2, patch_size, padded_size);

    fft2(padded1.data(), padded_size, padded_size);
    fft2(padded2.data(), padded_size, padded_size);

    std::vector<std::complex<float> > product(padded_size * padded_size);
    for (int i = 0; i < padded_size * padded_size; ++i) {
        product[i] = padded1[i] * std::conj(padded2[i]);
    }

    ifft2(product.data(), padded_size, padded_size);

    float correlation = product[0].real();

    float sumSquares1 = computeSumSquares(patch1);
    float sumSquares2 = computeSumSquares(patch2);

    return patch_size*patch_size*255*255*3-static_cast<int>(sumSquares1 + sumSquares2 - 2 * correlation);
}


int Image::ssdMask_fft(Point point1, Point point2, const Image& mask, int patch_size) const {
    std::vector<std::complex<float> > patch1 = extractPatchMasked(point1, mask, patch_size);
    std::vector<std::complex<float> > patch2 = extractPatchMasked(point2, mask, patch_size);

    int padded_size = nextPowerOf2(patch_size);
    std::vector<std::complex<float> > padded1 = padPatch(patch1, patch_size, padded_size);
    std::vector<std::complex<float> > padded2 = padPatch(patch2, patch_size, padded_size);

    fft2(padded1.data(), padded_size, padded_size);
    fft2(padded2.data(), padded_size, padded_size);

    std::vector<std::complex<float> > product(padded_size * padded_size);
    for (int i = 0; i < padded_size * padded_size; ++i) {
        product[i] = padded1[i] * std::conj(padded2[i]);
    }

    ifft2(product.data(), padded_size, padded_size);

    float correlation = product[0].real();

    float sumSquares1 = computeSumSquares(patch1);
    float sumSquares2 = computeSumSquares(patch2);

    return patch_size*patch_size*255*255*3-static_cast<int>(sumSquares1 + sumSquares2 - 2 * correlation);
}


int Image::getOverlapType(Point n1, Point n2, int patchSize) const {
    if (n1.first + patchSize/2 == n2.first) return 1; // node 1 on the left
    if (n2.first + patchSize/2 == n1.first) return 2; // node 2 on the left
    if (n1.second + patchSize/2 == n2.second) return 3; // n1 on the bottom
    if (n2.second + patchSize/2 == n1.second) return 4; // n2 on the bottom
    throw std::runtime_error("Invalid node configuration");
}



void Image::extractOverlapPatches(Point p1, Point p2, int patchSize, int overlapWidth, int overlapHeight, int overlapType,
                           std::vector<std::complex<float> >& patch1, std::vector<std::complex<float> >& patch2) const {
    patch1.resize(overlapWidth * overlapHeight);
    patch2.resize(overlapWidth * overlapHeight);

    for (int y = 0; y < overlapHeight; ++y) {
        for (int x = 0; x < overlapWidth; ++x) {
            int x1, y1, x2, y2;
            switch(overlapType) {
            case 1: // node 1 on the left
                x1 = p1.first + patchSize/2 - overlapWidth + 1 + x;
                y1 = p1.second - patchSize/2 + y;
                x2 = p2.first - patchSize/2 + x;
                y2 = p2.second - patchSize/2 + y;
                break;
            case 2: // node 2 on the left
                x1 = p1.first - patchSize/2 + x;
                y1 = p1.second - patchSize/2 + y;
                x2 = p2.first + patchSize/2 - overlapWidth + 1 + x;
                y2 = p2.second - patchSize/2 + y;
                break;
            case 3: // n1 on the bottom
                x1 = p1.first - patchSize/2 + x;
                y1 = p1.second + y;
                x2 = p2.first - patchSize/2 + x;
                y2 = p2.second - patchSize/2 + y;
                break;
            case 4: // n2 on the bottom
                x1 = p1.first - patchSize/2 + x;
                y1 = p1.second - patchSize/2 + y;
                x2 = p2.first - patchSize/2 + x;
                y2 = p2.second + y;
                break;
            default:
                throw std::runtime_error("Invalid overlap type");
            }

            float value1 = 0, value2 = 0;
            for (int d = 0; d < c; ++d) {
                value1 += (*this)(x1, y1, d);
                value2 += (*this)(x2, y2, d);
            }
            patch1[y * overlapWidth + x] = value1 / c;
            patch2[y * overlapWidth + x] = value2 / c;
        }
    }
}



double Image::computeFFTBasedSSD(const std::vector<std::complex<float> >& patch1, const std::vector<std::complex<float> >& patch2,
                          int width, int height) const {
    int paddedSize = nextPowerOf2(width);

    std::vector<std::complex<float> > padded1 = padPatch(patch1, width, height, paddedSize, paddedSize);
    std::vector<std::complex<float> > padded2 = padPatch(patch2, width, height, paddedSize, paddedSize);

    fft2(padded1.data(), paddedSize, paddedSize);
    fft2(padded2.data(), paddedSize, paddedSize);

    std::vector<std::complex<float> > product(paddedSize * paddedSize);
    for (size_t i = 0; i < product.size(); ++i) {
        product[i] = padded1[i] * std::conj(padded2[i]);
    }

    ifft2(product.data(), paddedSize, paddedSize);

    float correlation = product[0].real();
    float sumSquares1 = computeSumSquares(patch1);
    float sumSquares2 = computeSumSquares(patch2);

    return sumSquares1 + sumSquares2 - 2 * correlation;
}


std::vector<std::complex<float> > Image::extractPatchMasked(Point center, const Image& mask, int patch_size) const {
    std::vector<std::complex<float> > patch(patch_size * patch_size, 0);
    int half_patch = patch_size / 2;
    for (int y = -half_patch; y <= half_patch; ++y) {
        int j = center.second + y;
        if (j >= 0 && j < mask.height()) {
            for (int x = -half_patch; x <= half_patch; ++x) {
                int i = center.first + x;
                if (i >= 0 && i < mask.width()) {
                    if (mask(i, j, 0) < 128) {
                        float value = 0;
                        for (int d = 0; d < c; ++d) {
                            value += (*this)(i, j, d);
                        }
                        value /= c;  // Average over channels
                        patch[(y + half_patch) * patch_size + (x + half_patch)] = value;
                    }
                }
            }
        }
    }
    return patch;
}


std::vector<std::complex<float> > Image::extractPatch(Point center, int patch_size) const {
    std::vector<std::complex<float> > patch(patch_size * patch_size);
    int half_patch = patch_size / 2;
    for (int y = -half_patch; y <= half_patch; ++y) {
        for (int x = -half_patch; x <= half_patch; ++x) {
            if (x>=0 && x<width() && y>=0 && y<height()){
                float value = 0;
                for (int d = 0; d < c; ++d) {
                    value += (*this)(center.first + x, center.second + y, d);
                }
                value /= c;  // Average over channels
                patch[(y + half_patch) * patch_size + (x + half_patch)] = value;
            }
        }
    }
    return patch;
}


std::vector<std::complex<float> > Image::padPatch(const std::vector<std::complex<float> >& patch, int orig_size, int padded_size) const {
    std::vector<std::complex<float> > padded(padded_size * padded_size, 0);
    for (int y = 0; y < orig_size; ++y) {
        for (int x = 0; x < orig_size; ++x) {
            padded[y * padded_size + x] = patch[y * orig_size + x];
        }
    }
    return padded;
}


std::vector<std::complex<float> > Image::padPatch(const std::vector<std::complex<float> >& patch, int origWidth, int origHeight,
                                           int paddedWidth, int paddedHeight) const {
    std::vector<std::complex<float> > padded(paddedWidth * paddedHeight, 0);
    for (int y = 0; y < origHeight; ++y) {
        for (int x = 0; x < origWidth; ++x) {
            padded[y * paddedWidth + x] = patch[y * origWidth + x];
        }
    }
    return padded;
}
