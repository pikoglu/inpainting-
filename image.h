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
#include "node.h"


typedef std::pair<int, int> Point;
typedef std::pair<Point, int>  Belief;
typedef std::vector<Belief > ConfusionSet;

/// Float image class, with shallow copy for performance.
///
/// Copy constructor and operator= perform a shallow copy, so pixels are shared.
/// To perform a deep copy, use method clone().
/// There is a constructor taking array of pixels; no copy is done, make sure
/// the array exists during the lifetime of the image.
/// The channels of a color image are interlaced, meaning RGBRGB...
class Image {
    int* count; ///< number of shallow copies
    float* tab; ///< array of pixels
    int w, h, c; ///< width, height, channels
    void kill();
public:
    Image();
    Image(int width, int height, int channels=1);
    Image(float* pix, int width, int height, int channels=1);
    Image(const Image& I);
    ~Image() { kill(); }
    Image& operator=(const Image& I);
    Image clone() const;

    int width() const { return w; }
    int height() const { return h; }
    int channels() const { return c; }
    float  operator()(int i,int j,int d=0) const { return tab[(j*w+i)*c+d]; }
    float& operator()(int i,int j,int d=0)       { return tab[(j*w+i)*c+d]; }
    Image gray() const;

    //Felix
    int distance(int xp1, int yp1, int xp2, int yp2, int patch_size) const;
    int* listPatchCenters(int patchSize) const;
    int ssd(int xp1, int yp1, int xp2, int yp2, Image const &mask,int patch_size) const;
    Image createSSDImage(int patchSize,int ip) const;
    Image patchVisualization(int patchSize)const;
    int getPatchIndexFromCoordinates(int x, int y, int patchSize) const ;
    void initializeToBool();
    std::vector<Node> nodesOverMask( int patchsize) const;
    Image extendMask(int patchsize) const;
    void displayNodesOverMask(int* listNodes, int patchSize);
    void visualiseNodesAndVertices(std::vector<Node> v,int patchsize) ;
    bool isPatchInsideMask(int xp,int yp , int patchSize) const;
    std::vector<ConfusionSet> assignInitialPriority(const std::vector<Node>& nodes, const Image& maskExtended,const Image& mask,
                                                           int patchSize, int Lmin, int Lmax,
                                                           int thresholdConfusion, int thresholdSimilarity)const ;
    //


    // Filters (implemented in filters.cpp)
    Image gradX() const;
    void fillMinX(float vMin);
    void fillMaxX(float vMin);
    Image median(int radius) const;
    Image weightedMedian(const Image& guidance,
                         const Image& where, int vMin, int vMax,
                         int radius,
                         float sigmaSpace, float sigmaColor) const;
private:
    void fillX(float vMin, const float& (*cmp)(const float&,const float&));
    float dist2(int x1,int y1, int x2,int y2) const;
    void weighted_histo(std::vector<float>& tab, int x, int y, int radius,
                        int vMin, const Image& guidance,
                        float sSpace, float sColor) const;
};

bool save_disparity(const char* file_name, const Image& disparity,
                    int dMin, int dMax);


bool save_image(const char* fileName, const Image& img) ;
#endif
