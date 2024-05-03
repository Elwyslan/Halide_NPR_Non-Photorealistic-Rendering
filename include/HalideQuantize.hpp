#ifndef HALIDE_QUANTIZE
#define HALIDE_QUANTIZE

#include "Halide.h"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideQuantize : public Generator<HalideQuantize> {
    // TODO:
    // - Adicionar uma Func Float(32) de input e uma Func Float(32) de saída, ambas com duas dimensões
    // - Adicionar dois GeneratorParamas: um int chamado levels com valor padrão '10' e um float chamado phi_q com valor padrão '0.25'
    // - Criar método generate que computa a quantiza descrita no protótipo no Colab [vide o README para o link]
    // - Criar método schedule que seta estimativas para os valores de tamanho das dimensões dos Buffers para uso do autoscheduler
    public:
        Input<Func> in_img{ "in_img", Float(32), 2 };
        GeneratorParam<int> levels{ "levels", 10 };
        GeneratorParam<float> phi_q{ "phi_q", 0.25f };
        Output<Func> out_img{ "out_img", Float(32), 2 };
        
        void generate(){
            Expr step = 100 / levels;
            Expr q_nearest = floor(in_img(x,y)/step) * step;
            Expr diffs = in_img(x,y) - q_nearest;
            Expr tanh_term = tanh(phi_q * diffs);
            
            //q_nearest + (step / 2) * np.tanh(phi_q * (c - q_nearest))
            out_img(x,y) = q_nearest + (step / 2) * tanh_term;
        }
        
        void schedule(){
            if (using_autoscheduler()) {
                in_img.set_estimates({{0, 6000}, {0, 4000}});
                out_img.set_estimates({{0, 6000}, {0, 4000}});
                return;
            }
        }

    private:
        Var x{"x"}, y{"y"};
};

#endif

