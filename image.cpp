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

int Image::ssd(int xp1, int yp1, int xp2, int yp2, int patch_size) const{
    int half_patch = patch_size / 2;
    int sum = 0;

    for (int y = -half_patch; y <= half_patch; y++) {
        for (int x = -half_patch; x <= half_patch; x++) {
            for (int channel = 0; channel < c; channel++) {
                int diff = (*this)(xp1 + x, yp1 + y, channel) - (*this)(xp2 + x, yp2 + y, channel);
                sum += diff * diff;
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
        int ssd_patch = this->ssd(xp, yp, x2, y2, patchSize);
        ssdMax = std::max(ssdMax, static_cast<float>(ssd_patch)); //Attention il est sans doute nécessaire de
    }

    //etape de normalisation
    for (int j = 0; j < numpatchs; ++j) {
        int x2 = centers[2 * j];
        int y2 = centers[2 * j + 1];
        int ssd_patch = this->ssd(xp, yp, x2, y2, patchSize);
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


#include <algorithm> // pour std::clamp

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


void Image::getPatchesOverMask(Image &boolImage, const Image &imageMask, int patchsize) {
    int width = imageMask.width();
    int height = imageMask.height();

    int numberUniquePatchWidth = (2 * width) / patchsize - 1;
    int numberUniquePatchHeight = (2 * height) / patchsize - 1;

    // Il existe surement des manières plus rapide de faire ça, notamment en ajoutant pour un pixels les 4 patchs associés
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (imageMask(x, y) >128) {
                int patchX = (2 * x) / patchsize;
                int patchY = (2 * y) / patchsize;


                if (patchX < numberUniquePatchWidth && patchY < numberUniquePatchHeight) {
                    boolImage(patchX, patchY) = 255;
                }

                x += patchsize / 2;
            }
        }
    }
}
