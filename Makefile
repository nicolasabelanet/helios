CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# create list of all spv files and set as dependency
vert_sources = $(shell find ./shaders -type f -name "*.vert")
vert_obj_files = $(patsubst %.vert, %.vert.spv, $(vert_sources))
frag_sources = $(shell find ./shaders -type f -name "*.frag")
frag_obj_files = $(patsubst %.frag, %.frag.spv, $(frag_sources))

TARGET = a.out
$(TARGET): $(vert_obj_files) $(frag_obj_files)
$(TARGET): *.cpp *.hpp
	echo $(CPATH)
	echo $(LIBRARY_PATH)
	g++ $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

# make shader targets
%.spv: %
	glslc $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f *.spv
