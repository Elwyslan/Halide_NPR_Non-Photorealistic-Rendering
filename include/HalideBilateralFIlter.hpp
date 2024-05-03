#ifndef HALIDE_BF_HPP
#define HALIDE_BF_HPP

#include "Halide.h"
#include "gaussian.hpp"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideBilateralFilter : public Generator<HalideBilateralFilter> {
    public:
        Input<Func> img_input{"img_input_bf", Float(32), 3};

        GeneratorParam<int> kernel_size{"kernel_size", 5};
        GeneratorParam<float> sigma_range{"sigma_range", 3.f};
        GeneratorParam<float> sigma_spatial{"sigma_spatial", 4.25f};

        Output<Func> img_output{"img_output_bf", Float(32), 3};

        void generate() {
            RDom kernel(-(kernel_size / 2), kernel_size, -(kernel_size / 2), kernel_size, "kernel");

            diff(x, y, k, l, c) = pow(absd(img_input(x, y, c), img_input(x + k, y + l, c)), 2);

            weights_spatial(k) = gaussian(k, sigma_spatial);
            weights_range(k) = gaussian(k, sigma_range);
            weights(x, y, k, l, c) = weights_spatial(k) * weights_spatial(l) * weights_range(i32(diff(x, y, k, l, c)));
            sum_w(x, y, c) += weights(x, y, kernel.x, kernel.y, c);

            img_output(x, y, c) += weights(x, y, kernel.x, kernel.y, c) * img_input(x + kernel.x, y + kernel.y, c) / sum_w(x, y, c);
        }

        void schedule() {
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 6000}, {0, 4000}, {0, 3}});
                img_output.set_estimates({{0, 6000}, {0, 4000}, {0, 3}});
            } else {
                // inline computation
            }
        }

    private:
        Var x{"x"}, y{"y"}, k{"k"}, l{"l"}, c{"c"};
        Func diff{"diff"};
        Func weights_spatial{"weights_spatial"}, weights_range{"weights_range"};
        Func weights{"weights"}, sum_w{"sum_w"};
};

#endif
