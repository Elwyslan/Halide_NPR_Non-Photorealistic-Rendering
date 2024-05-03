#ifndef GAUSSIAN_HPP
#define GAUSSIAN_HPP

#include "Halide.h"

using namespace Halide;

Expr gaussian(Expr in, Expr sigma) {
    return exp(-(in * in) / (2.0f * sigma * sigma));
}

int get_kernel_width(float sigma) {
    // From OpenCV
    // https://github.com/opencv/opencv/blob/625eebad54a34a7bdad6812f3e9ec050a1b3adc5/modules/features2d/src/kaze/AKAZEFeatures.cpp#L107
    int ksize = int(2.0f * (1.0f + (sigma - 0.8f) / (0.3f))) + 1;
    ksize |= 1;
    return ksize;
}

#endif