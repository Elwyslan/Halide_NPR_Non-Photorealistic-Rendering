#include <iostream>
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "npr.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char**argv) {
    if (argc < 3) {
        cout << "Usage: bin/main <path/to/input> <path/to/output>" << endl;
        return -1;
    }

    const string path_in = argv[1];
    const string path_out = argv[2];

    Buffer<uint8_t> img_input = load_image(path_in);
    Buffer<uint8_t> img_output = Buffer<uint8_t>::make_with_shape_of(img_input);

    npr(img_input, img_output);

    save_image(img_output, path_out);

    cout<<"################\n"<<path_out<<"\n################";
    return 0;
}