#ifndef HALIDE_NPR
#define HALIDE_NPR

#include "Halide.h"

#include "HalideBilateralFIlter.hpp"
#include "HalideRGB2Lab.hpp"
#include "HalideQuantize.hpp"
#include "HalideXDoG.hpp"
#include "HalideElementwiseMult.hpp"
#include "HalideLab2RGB.hpp"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideNPR : public Generator<HalideNPR> {
    public:
        // A entrada é um Buffer uint8 com 3 canais (RGB)
        Input<Buffer<uint8_t>> img_input{"img_input_npr", 3};

        // A saída é um Buffer uint8 com 3 canais (RGB)
        Output<Buffer<uint8_t>> img_output{"img_output_npr", 3};

        void generate() {
            // Padding com reflexão interna da imagem
            img_input_bound = BoundaryConditions::mirror_interior(img_input);
            
            // Conversão para Lab.
            // Criando o Generator
            rgb2lab = create<HalideRGB2Lab>();
            // Aplicando na Func apropriada
            rgb2lab->apply(img_input_bound);

            // Filtro bilateral na imagem Lab (rgb2lab->img_output)
            // Criando o Generator
            bilateral_filter = create<HalideBilateralFilter>();
            // Setando os GeneratorParams
            bilateral_filter->kernel_size.set(5);
            bilateral_filter->sigma_range.set(3.0f);
            bilateral_filter->sigma_spatial.set(4.25f);
            // Aplicando na Func apropriada (rgb2lab->img_output)
            bilateral_filter->apply(rgb2lab->img_output);
            
            // Splitting dos canais. 
            // 'l' é usado em outras funções
            // 'a' e 'b' seram apenas mergeados no final com a 'l' alterada
            Expr l, a, b;
            l = bilateral_filter->img_output(x, y, 0);
            a = bilateral_filter->img_output(x, y, 1);
            b = bilateral_filter->img_output(x, y, 2);

            // Criando uma Func para poder passar o canal 'l' como entrada aos outros Generators
            l_channel(x, y) = l;
            a_channel(x, y) = a;
            b_channel(x, y) = b;

            // Crie o ponteiro de generator 'quantize'. Veja acima como base
            // Dê o valor 10 ao GeneratorParam 'levels'. Veja acima como base
            // Dê o valor 0.25 (float) ao GeneratorParam 'phi_q'. Veja acima como base
            // Aplique a quantização na Func correta. Veja os slides ou protótipo para saber qual
            
            quantize = create<HalideQuantize>();
            quantize->levels.set(10);
            quantize->phi_q.set(0.25f);
            quantize->apply(l_channel);
    
            // Crie o ponteiro de generator 'xdog'. Veja acima como base
            // Dê o valor 0.5 (float) ao GeneratorParam 'sigma'. Veja acima como base
            // Dê o valor 7.0 (float) ao GeneratorParam 'k'. Veja acima como base
            // Dê o valor 0.95 (float) ao GeneratorParam 'gamma'. Veja acima como base
            // Dê o valor 5.0 (float) ao GeneratorParam 'phi_e'. Veja acima como base
            // Aplique o XDoG na Func correta. Veja os slides ou protótipo para saber qual

            xdog = create<HalideXDoG>();
            xdog->sigma.set(0.5f);
            xdog->k.set(7.0f);
            xdog->gamma.set(0.95f);
            xdog->phi_e.set(5.0f);
            xdog->apply(l_channel);

            // Crie o ponteiro de generator 'elementwise_multiplication'. Veja acima como base
            // Aplique a multiplicação nas Funcs corretas. Veja os slides ou protótipo para saber quais

            elementwise_multiplication = create<HalideElementwiseMult>();
            elementwise_multiplication->apply(xdog->out_img, quantize->out_img);

            // Merging dos canais
            // Canal 'l' novo é o resultado a aplicação do 'elementwise_multiplication'
            final_lab(x, y, c) = mux(c, 
                {elementwise_multiplication->img_output(x, y), a, b}
            );

            // Conversão para RGB
            // Criando o Generator
            lab2rgb = create<HalideLab2RGB>();
            // Aplicando na Func apropriada
            lab2rgb->apply(final_lab);

            // Salvando a imagem de saída
            img_output(x, y, c) = u8(lab2rgb->img_output(x, y, c));
        }

        void schedule() {
            // Setando as dimensões para uso do autoscheduler
            if (using_autoscheduler()) {
                img_input.set_estimates({{0, 6000}, {0, 4000}, {0, 3}});
                img_output.set_estimates({{0, 6000}, {0, 4000}, {0, 3}});
            } else {
                // inline computation
            }
        }

    private:
        Var x{"x_npr"}, y{"y_npr"}, c{"c_npr"};
        Func img_input_bound{"img_input_bound"};
        Func l_channel{"l_channel"};
        Func a_channel{"a_channel"};
        Func b_channel{"b_channel"};
        Func final_lab{"final_lab"};

        std::unique_ptr<HalideRGB2Lab> rgb2lab;
        std::unique_ptr<HalideBilateralFilter> bilateral_filter;
        std::unique_ptr<HalideQuantize> quantize;
        std::unique_ptr<HalideXDoG> xdog;
        std::unique_ptr<HalideElementwiseMult> elementwise_multiplication;
        std::unique_ptr<HalideLab2RGB> lab2rgb;
};

#endif
