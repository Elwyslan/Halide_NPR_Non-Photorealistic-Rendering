CXX = g++
CXX_FLAGS = -std=c++20 -fno-rtti

GENERATOR_DEPS = ${HALIDE_ROOT}/tools/GenGen.cpp
GENERATOR_OUTPUTS = o,h,stmt_html
INCLUDES = -I${HALIDE_ROOT}/include -I${HALIDE_ROOT}/tools -Iinclude -Ibin
LIBS = -L${HALIDE_ROOT}/lib
LIB_FLAGS = -lHalide -lpthread -ldl -lz -ltinfo
IMAGE_IO_FLAGS = -ljpeg `libpng-config --cflags --ldflags`

TARGET=host

all: test

gens: bin/npr.generator bin/npr.o bin/halide_quantizer.generator bin/halide_quantizer.o bin/halide_ew_mult.generator bin/halide_ew_mult.o

clean:
	@rm -rf bin outputs

bin/halide_quantizer.generator: src/registration/HalideNPR.cpp
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@
	
bin/halide_ew_mult.generator: src/registration/HalideNPR.cpp
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/npr.generator: src/registration/HalideNPR.cpp
	@mkdir -p $(@D)
	@$(CXX) $^ $(GENERATOR_DEPS) $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) -o $@

bin/halide_quantizer.o: bin/halide_quantizer.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f halide_quantizer -g halide_quantizer target=$(TARGET)

bin/halide_ew_mult.o: bin/halide_ew_mult.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f halide_ew_mult -g halide_ew_mult target=$(TARGET)

bin/npr.o: bin/npr.generator
	@$^ -e $(GENERATOR_OUTPUTS) -o $(@D) -f npr -g npr target=$(TARGET) autoscheduler=Mullapudi2016 -p ${HALIDE_ROOT}/lib/libautoschedule_mullapudi2016.so

bin/main: src/main.cpp bin/npr.o
	@$(CXX) $^ $(CXX_FLAGS) $(INCLUDES) $(LIBS) $(LIB_FLAGS) $(IMAGE_IO_FLAGS) -o $@

test: bin/main
	@mkdir -p outputs
	@for i in `find "inputs/" -type f -name "*.png"`; do $^ $$i outputs/`basename $$i`; done
	@pytest

.PHONY: all clean