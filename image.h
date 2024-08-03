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
    int* tab; ///< array of pixels
    int w, h, c; ///< width, height, channels
    void kill();
public:
    Image();
    Image(int width, int height, int channels=1);
    Image(int* pix, int width, int height, int channels=1);
    Image(const Image& I);
    ~Image() { kill(); }
    Image& operator=(const Image& I);
    Image clone() const;

    int width() const { return w; }
    int height() const { return h; }
    int channels() const { return c; }
    int  operator()(int i,int j,int d) const { return tab[(j*w+i)*c+d]; }
    int& operator()(int i,int j,int d)       { return tab[(j*w+i)*c+d]; }
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



    bool isPatchInsideMask(Point patchPoint , int patchSize) const;

    Image simplifyMaskToOnePixel(int x1,int y1,int x2,int y2)const;



    int ssd_fft(Point point1, Point point2, int patch_size) const {
        // 1. Extract patches
        std::vector<std::complex<float> > patch1 = extractPatch(point1, patch_size);
        std::vector<std::complex<float> > patch2 = extractPatch(point2, patch_size);

        // 2. Pad patches to nearest power of 2
        int padded_size = nextPowerOf2(patch_size);
        std::vector<std::complex<float> > padded1 = padPatch(patch1, patch_size, padded_size);
        std::vector<std::complex<float> > padded2 = padPatch(patch2, patch_size, padded_size);

        // 3. Compute 2D FFTs
        fft2(padded1.data(), padded_size, padded_size);
        fft2(padded2.data(), padded_size, padded_size);

        // 4. Multiply FFTs (with complex conjugate)
        std::vector<std::complex<float> > product(padded_size * padded_size);
        for (int i = 0; i < padded_size * padded_size; ++i) {
            product[i] = padded1[i] * std::conj(padded2[i]);
        }

        // 5. Compute inverse FFT
        ifft2(product.data(), padded_size, padded_size);

        // 6. Extract relevant part (this is now our correlation)
        float correlation = product[0].real();

        // 7. Compute sums of squares from original patches
        float sumSquares1 = computeSumSquares(patch1);
        float sumSquares2 = computeSumSquares(patch2);

        // 8. Combine results
        return static_cast<int>(sumSquares1 + sumSquares2 - 2 * correlation);
    }

private:
    // Helper functions
    std::vector<std::complex<float> > extractPatch(Point center, int patch_size) const {
        std::vector<std::complex<float> > patch(patch_size * patch_size);
        int half_patch = patch_size / 2;
        for (int y = -half_patch; y <= half_patch; ++y) {
            for (int x = -half_patch; x <= half_patch; ++x) {
                int i = center.first + x;
                int j = center.second + y;
                if (i >= 0 && i < w && j >= 0 && j < h) {
                    float value = 0;
                    for (int d = 0; d < c; ++d) {
                        value += (*this)(i, j, d);
                    }
                    value /= c;  // Average over channels
                    patch[(y + half_patch) * patch_size + (x + half_patch)] = value;
                }
            }
        }
        return patch;
    }

    std::vector<std::complex<float> > padPatch(const std::vector<std::complex<float> >& patch, int orig_size, int padded_size) const {
        std::vector<std::complex<float> > padded(padded_size * padded_size, 0);
        for (int y = 0; y < orig_size; ++y) {
            for (int x = 0; x < orig_size; ++x) {
                padded[y * padded_size + x] = patch[y * orig_size + x];
            }
        }
        return padded;
    }

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




