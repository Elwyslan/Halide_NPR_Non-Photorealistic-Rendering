#ifndef HALIDE_LAB2RGB
#define HALIDE_LAB2RGB

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideLab2RGB : public Generator<HalideLab2RGB> {
    public:
        Input<Func> img_input{"img_input_l2r", Float(32), 3};

        Output<Buffer<uint8_t>> img_output{"img_output_l2r", 3};

        void generate() {
            Expr L = img_input(x, y, 0);
            Expr a = img_input(x, y, 1);
            Expr b = img_input(x, y, 2);

            Expr Y_ = (L + 16.f) / 116.f;
            Expr X_ = (a / 500.f) + Y_;
            Expr Z_ = Y_ - (b / 200.f);

            Expr X_cubed = X_ * X_ * X_;
            Expr Y_cubed = Y_ * Y_ * Y_;
            Expr Z_cubed = Z_ * Z_ * Z_;

            compX(x, y) = select(X_cubed > 0.008856f, X_cubed, ((X_ + 16.f) / 116.f) / 7.787f) * 0.950456f;
            compY(x, y) = select(Y_cubed > 0.008856f, Y_cubed, ((Y_ + 16.f) / 116.f) / 7.787f);
            compZ(x, y) = select(Z_cubed > 0.008856f, Z_cubed, ((Z_ + 16.f) / 116.f) / 7.787f) * 1.088754f;

            Expr R = 3.240479f * compX(x, y) - 1.53715f * compY(x, y) - 0.498535f * compZ(x, y);
            Expr G = -0.969256f * compX(x, y) + 1.875991f * compY(x, y) + 0.041556f * compZ(x, y);
            Expr B = 0.055648f * compX(x, y) - 0.204043f * compY(x, y) + 1.057311f * compZ(x, y);

            img_output(x, y, c) = u8_sat(mux(c, {
                R, 
                G, 
                B
            }) * 255.f);
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
        Var x{"x_l2r"}, y{"y_l2r"}, c{"c_l2r"};
        Func compX{"compute_X"}, compY{"compute_Y"}, compZ{"compute_Z"};

        Expr f(Expr t) {
            return select(t > 0.008856f, pow(t, 0.33333f), 7.787f * t + 0.137931f);
        }
};

#endif
