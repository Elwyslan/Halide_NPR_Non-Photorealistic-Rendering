#include "HalideNPR.hpp"

HALIDE_REGISTER_GENERATOR(HalideNPR, npr)
HALIDE_REGISTER_GENERATOR(HalideQuantize, halide_quantizer)
HALIDE_REGISTER_GENERATOR(HalideElementwiseMult, halide_ew_mult)