/**
 * @file image.h
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

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <array>
#include <complex>
#include "fft.h"
#include <stdexcept>

typedef std::array<int, 5> Messages;
typedef std::pair<int, int> Point;
typedef std::pair<Point, Messages> Belief;
typedef std::vector<Belief> ConfusionSet;

/// Float image class, with shallow copy for performance.
///
/// Copy constructor and operator= perform a shallow copy, so pixels are shared.
/// To perform a deep copy, use method clone().
/// There is a constructor taking array of pixels; no copy is done, make sure
/// the array exists during the lifetime of the image.
/// The channels of a color image are interlaced, meaning RGBRGB...
class Image {
    int* count; ///< number of shallow copies
    unsigned char* tab; ///< array of pixels
    int w, h, c; ///< width, height, channels
    void kill();
public:
    Image();
    Image(int width, int height, int channels=1);
    Image(unsigned char* pix, int width, int height, int channels=1);
    Image(const Image& I);
    ~Image() { kill(); }
    Image& operator=(const Image& I);
    Image clone() const;

    int width() const { return w; }
    int height() const { return h; }
    int channels() const { return c; }
    unsigned char operator()(int i, int j, int d) const {
        if (i < 0 || i >= w || j < 0 || j >= h || d < 0 || d >= c) {
            throw std::out_of_range("Index out of bounds");
        }
        return tab[(j * w + i) * c + d];
    }

    // Opérateur pour accès en écriture avec vérification des limites
    unsigned char& operator()(int i, int j, int d) {
        if (i < 0 || i >= w || j < 0 || j >= h || d < 0 || d >= c) {
            throw std::out_of_range("Index out of bounds");
        }
        return tab[(j * w + i) * c + d];
    }
    Image gray() const;

    //Felix
    int distance(int xp1, int yp1, int xp2, int yp2, int patch_size) const;
    int* listPatchCenters(int patchSize) const;
    int ssdMask(Point point1, Point point2, Image const &mask,int patch_size) const;
    int ssd(Point point1, Point point2,int patch_size) const;

    Image createSSDImage(int patchSize,int ip) const;
    Image patchVisualization(int patchSize)const;
    int getPatchIndexFromCoordinates(int x, int y, int patchSize) const ;
    void initializeToBool();

    Image extendMask(int patchsize) const;

    double ssdOverlap(Point n1, Point n2, Point p1, Point p2, int patchSize, int w0)const ;

    Image extendPatern(int w,int h) const;

    Image extendedPaternMask(int w,int h)const;


    bool isPatchInsideMask(Point patchPoint , int patchSize) const;

    Image simplifyMaskToOnePixel(int x1,int y1,int x2,int y2)const;

    double ssdOverlap_fft(Point n1, Point n2, Point p1, Point p2, int patchSize, int w0) const ;

    int ssd_fft(Point point1, Point point2, int patch_size) const ;

    int ssdMask_fft(Point point1, Point point2, const Image& mask, int patch_size) const ;




private:

    int getOverlapType(Point n1, Point n2, int patchSize) const ;


    void extractOverlapPatches(Point p1, Point p2, int patchSize, int overlapWidth, int overlapHeight, int overlapType,
                               std::vector<std::complex<float> >& patch1, std::vector<std::complex<float> >& patch2) const ;

    double computeFFTBasedSSD(const std::vector<std::complex<float> >& patch1, const std::vector<std::complex<float> >& patch2,
                              int width, int height) const ;



    std::vector<std::complex<float> > extractPatchMasked(Point center, const Image& mask, int patch_size) const ;

    std::vector<std::complex<float> > extractPatch(Point center, int patch_size) const ;


    std::vector<std::complex<float> > padPatch(const std::vector<std::complex<float> >& patch, int orig_size, int padded_size) const ;


    std::vector<std::complex<float> > padPatch(const std::vector<std::complex<float> >& patch, int origWidth, int origHeight,
                                               int paddedWidth, int paddedHeight) const ;



    int nextPowerOf2(int n) const {
        return static_cast<int>(std::pow(2, std::ceil(std::log2(n))));
    }

    float computeSumSquares(const std::vector<std::complex<float> >& patch) const {
        float sum = 0;
        for (const auto& val : patch) {
            sum += std::norm(val);
        }
        return sum;
    }

};

bool save_disparity(const char* file_name, const Image& disparity,
                    int dMin, int dMax);


bool save_image(const char* fileName, const Image& img) ;


#endif




