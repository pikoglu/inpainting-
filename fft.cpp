#include "fft.h"
#include <cassert>

// Discrete Fourier transform a l'index k du signal f de longueur n.
// s=-1 pour DFT directe, s=+1 pour DFT inverse.
std::complex<float> dft(const std::complex<float> f[], int n, int k, float s) {
    std::complex<float> g=0;
    for(int j=0; j<n; j++)
        g += f[j]*std::polar(1.0f, s*(float)M_PI*(2*j)/n*k);
    return g/(float)sqrt(float(n));
}

// Fast Fourier transform du signal f(deb:pas:fin) (notation Matlab).
// s=-1 pour DFT directe, s=+1 pour DFT inverse.
// buffer est utilise comme tableau temporaire, sa longueur doit etre au moins
// celle de f.
void fft_main(std::complex<float> f[], int deb, int pas, int fin, float s,
              std::complex<float> buffer[]) {
    if(deb==fin)
        return;
    const int n = 1+(fin-deb)/pas;
    assert(n%2==0);
    fft_main(f, deb, 2*pas, fin-pas, s, buffer);
    fft_main(f, deb+pas, 2*pas, fin, s, buffer);
    for(int i=deb, j=0; i<=fin; i+=pas, j++)
        buffer[j] = f[i];
    const int moitie = pas*n/2;
    const std::complex<float> unit=std::polar(1.0f, s*(float)M_PI*2.0f/n);
    std::complex<float> twiddle=1.0f;
    for(int i=0, j=deb; i<n; i+=2, j+=pas) {
        f[j]        = buffer[i] + twiddle*buffer[i+1];
        f[j+moitie] = buffer[i] - twiddle*buffer[i+1];
        twiddle *= unit;
    }
}

// Divise tous les coefficients de f par la racine carree de sa longueur n.
void normalize(std::complex<float> f[], int n, float div) {
    for(int i=0; i<n; i++)
        f[i] /= div;
}

// FFT du signal f de longueur n.
void fft(std::complex<float> f[], int n) {
    std::complex<float>* buffer = new std::complex<float>[n];
    fft_main(f, 0, 1, n-1, -1.0f, buffer);
    delete [] buffer;
    normalize(f, n, (float)sqrt((float)n));
}

// FFT inverse du signal f de longueur n.
void ifft(std::complex<float> f[], int n) {
    std::complex<float>* buffer = new std::complex<float>[n];
    fft_main(f, 0, 1, n-1, +1.0f, buffer);
    delete [] buffer;
    normalize(f, n, (float)sqrt((float)n));
}

// FFT du signal 2D f de dimension wxh.
void fft2(std::complex<float> f[], int w, int h) {
    std::complex<float>* buffer = new std::complex<float>[std::max(w,h)];
    for(int i=0; i<h; i++)
        fft_main(f, i*w, 1, i*w+w-1,   -1.0f, buffer);
    for(int j=0; j<w; j++)
        fft_main(f, j,  w,  j+(h-1)*w, -1.0f, buffer);
    delete [] buffer;
    normalize(f, w*h, (float)sqrt((float)w*h));
}

// FFT inverse du signal 2D f de dimentsion wxh.
void ifft2(std::complex<float> f[], int w, int h) {
    std::complex<float>* buffer = new std::complex<float>[std::max(w,h)];
    for(int i=0; i<h; i++)
        fft_main(f, i*w, 1, i*w+w-1,   +1.0f, buffer);
    for(int j=0; j<w; j++)
        fft_main(f, j,   w, j+(h-1)*w, +1.0f, buffer);
    delete [] buffer;
    normalize(f, w*h, (float)sqrt((float)w*h));
}
