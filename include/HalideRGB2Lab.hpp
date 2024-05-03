#ifndef HALIDE_RGB2LAB
#define HALIDE_RGB2LAB

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideRGB2Lab : public Generator<HalideRGB2Lab> {
    public:
        Input<Func> img_input{"img_input_r2l", UInt(8), 3};
        Output<Func> img_output{"img_output_r2l", Float(32), 3};

        void generate() {
            Expr scale = 1 / 255.f;
            input_f32(x, y, c) = f32(img_input(x, y, c)) * scale;

            Expr R = input_f32(x, y, 0);
            Expr G = input_f32(x, y, 1);
            Expr B = input_f32(x, y, 2);

            Expr X = 0.412453f * R + 0.357580f * G + 0.180423f * B;
            Expr Y = 0.212671f * R + 0.715160f * G + 0.072169f * B;
            Expr Z = 0.019334f * R + 0.119193f * G + 0.950227f * B;

            Expr Xn = X / 0.950456f;
            Expr Zn = Z / 1.088754f;

            compL(x, y) = select(Y > 0.008856f, 
                116.f * pow(Y, 0.33333f) - 16.f,
                903.3f * Y
            );

            Expr fX = f(Xn);
            Expr fY = f(Y);
            Expr fZ = f(Zn);
            compa(x, y) = 500.f * (fX - fY);
            compb(x, y) = 200.f * (fY - fZ);

            img_output(x, y, c) = mux(c, {compL(x, y), compa(x, y), compb(x, y)});
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
        Var x{"x_r2l"}, y{"y_r2l"}, c{"c_r2l"};
        Func input_f32{"input_f32_r2l"};
        Func compL{"compute_L"}, compa{"compute_a"}, compb{"compute_b"};

        Expr f(Expr t) {
            return select(t > 0.008856f, pow(t, 0.33333f), 7.787f * t + 0.137931f);
        }
};

#endif