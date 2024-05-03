#ifndef HALIDE_XDOG_HPP
#define HALIDE_XDOG_HPP

#include "Halide.h"
#include "gaussian.hpp"

using namespace Halide;
using namespace Halide::ConciseCasts;

class HalideXDoG : public Generator<HalideXDoG> {
    // TODO:
    // - Adicionar uma Func Float(32) de input e uma Func Float(32) de saída, ambas com duas dimensões
    // - Adicionar quatro GeneratorParamas (todos são do tipo float): 
    //    - sigma com valor padrão '0.5'
    //    - k com valor padrão '7.0'
    //    - gamma com valor padrão '0.95'
    //    - phi_e com valor padrão '5.0'
    // - Criar método generate que computa a XDoG descrita no protótipo no Colab [vide o README para o link]
    //    *  Use o exemplo de DoG da semana 04 como base. Não é necessário criar um generator separado para o DoG puro, apenas adiciona ao exemplo se usar
    // - Criar método schedule que seta estimativas para os valores de tamanho das dimensões dos Buffers para uso do autoscheduler

    // Dica: não é necessário explicitar padding aqui pois a Func que deve ser entrada já está coberta por uma BoundaryCondition
    public:
        Input<Func> in_img{ "in_img", Float(32),2 };
        GeneratorParam<float> sigma{ "sigma", 0.5f };
        GeneratorParam<float> k{ "k", 7.0f };
        GeneratorParam<float> gamma{ "gamma", 0.95f };
        GeneratorParam<float> phi_e{ "phi_e", 5.0f };
        Output<Func> out_img{ "out_img", Float(32),2 };

        Func convolution(Func f, Func hx, Expr kernel_width,Expr kernel_height){
            Var x2("x2"),y2("y2");
            Func convolved("convolved");
            RDom r_c (0,kernel_width,0,kernel_height);
            convolved(x,y) += ( hx(r_c.x, r_c.y)*f(x+r_c.x-(kernel_width/2),y+r_c.y-(kernel_height/2)));
            return convolved;
        }

        void generate(){
            /* D O G */
            int kernel_width = get_kernel_width(sigma.value());
            int half_w = (int)floor(kernel_width/2.0f);
            float kernel_sum = 0.0f;
            float xx, yy, f_exponent;
            l_gauss_kernel(x, y) = 0.0f;
            for (int i=0; i<kernel_width; i++){
                for (int j=0; j<kernel_width; j++){
                    xx = (float) ((i-half_w)*(i-half_w));
                    yy = (float) ((j-half_w)*(j-half_w));
                    f_exponent = exp(-(xx + yy)/(2.0f*sigma*sigma)); //gauss_k(x,y) = e^(-(x²+y²)/2*sigma²)
                    Expr exponent = f_exponent;
                    l_gauss_kernel(i,j) = exponent;
                    kernel_sum += f_exponent;
                }
            }
            low_blurred(x,y) = convolution(in_img,
                                           l_gauss_kernel,
                                           kernel_width,kernel_width)(x,y) / kernel_sum;

            kernel_width = get_kernel_width(sigma.value()*k.value());
            half_w = (int)floor(kernel_width/2.0f);
            kernel_sum = 0.0f;
            h_gauss_kernel(x, y) = 0.0f;
            for (int i=0; i<kernel_width; i++){
                for (int j=0; j<kernel_width; j++){
                    xx = (float) ((i-half_w)*(i-half_w));
                    yy = (float) ((j-half_w)*(j-half_w));
                    f_exponent = exp(-(xx + yy)/(2.0f*sigma*sigma*k)); //gauss_k(x,y) = e^(-(x²+y²)/2*sigma²)
                    Expr exponent = f_exponent;
                    h_gauss_kernel(i,j) = exponent;
                    kernel_sum += f_exponent;
                }
            }
            high_blurred(x,y) = convolution(in_img,
                                            h_gauss_kernel,
                                            kernel_width,kernel_width)(x,y) / kernel_sum;

            dog(x, y) = low_blurred(x, y) - gamma * high_blurred(x, y);
            
            /* X D O G */
            //edges[diff > 0] = 1
            //edges[diff <= 0] = 1 + np.tanh(phi_e * diff)[diff <= 0]
            Func thres{"threshold_xdog"};
            thres(x, y) = select(dog(x, y)>0.0f, 1.0f,
                                 dog(x, y)<=0.0f, 1.0f + tanh(phi_e * dog(x, y)), dog(x, y));
            
            out_img(x,y) = f32(clamp(thres(x,y), 0.0f, 1.0f));
        }
        
        void schedule(){
            if (using_autoscheduler()) {
                in_img.set_estimates({{0, 512}, {0, 512}});
                out_img.set_estimates({{0, 512}, {0, 512}});
                return;
            }
        }

    private:
        Var x{"x"}, y{"y"};
        Func l_gauss_kernel{"low_blur_gauss_kernel"},
             h_gauss_kernel{"high_blur_gauss_kernel"},
             low_blurred{"halide_low_dog_gauss_blur"},
             high_blurred{"halide_high_dog_gauss_blur"},
             dog{"final_dog"};
};
#endif
