GRAPHICSLIBS= -lassimp -lGLEW -lglut -lGLU -lGL 
INCUDES=
CC=g++
LD=g++
CC_FLAGS=-std=c++11 

all: bin/hellorhombus bin/hellogl3 bin/firstcamera bin/firstmesh


bin/%: obj/%.o
	$(LD) $(CC_FLAGS)  $< -o $@ $(GRAPHICSLIBS)


obj/%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $(INCLUDES) $< -o $@


clean:
	rm -f bin/*
	rm -f obj/*