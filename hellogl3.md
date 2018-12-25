### OpenGL3 initialization
There are essentially 4 core things that need to be setup for a basic OpenGL display.
1. The OpenGL Context
2. Buffers for vertex attributes (such as position, color and normals)
3. Shaders for processing the vertices and fragments (at a minimum, these two are required).
4. The rendering loop. Unfortunately, this needs to be a C-style procedural function (with no arguments).
