GRAPHICSLIBS=  -lGLEW -lglut -lGLU -lGL
INCUDES=
CC=g++
LD=g++
CC_FLAGS=-std=c++11

all: bin/hellorhombus


bin/%: obj/%.o
	$(LD) $(CC_FLAGS)  $< -o $@ $(GRAPHICSLIBS)


obj/%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $(INCLUDES) $< -o $@
