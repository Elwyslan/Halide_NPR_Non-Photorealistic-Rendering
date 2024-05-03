#ifndef HALIDE_ELEMENTWISE_MULT
#define HALIDE_ELEMENTWISE_MULT

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideElementwiseMult : public Generator<HalideElementwiseMult> {
    // TODO:
    // - Adicionar duas Funcs Float(32) de input e uma Func Float(32) de saída, todas com duas dimensões
    // - Criar método generate que computa o produto entre os pixels das duas Funcs de entrada e salva na saída
    // - Criar método schedule que seta estimativas para os valores de tamanho das dimensões dos Buffers para uso do autoscheduler
    public:
        Input<Func> in_0{ "in_0", Float(32),2 };
        Input<Func> in_1{ "in_1", Float(32), 2 };
        Output<Func> img_output{ "img_output", Float(32), 2};

        void generate(){
            img_output(x, y) = in_0(x,y) * in_1(x,y);
        }

        void schedule(){
            if (using_autoscheduler()) {
                in_0.set_estimates({{0, 6000}, {4000, 512}});
                in_1.set_estimates({{0, 6000}, {4000, 512}});
                img_output.set_estimates({{0, 6000}, {0, 4000}});
                return;
            }
        }

    private:
        Var x{"x"}, y{"y"};
};


#endif
