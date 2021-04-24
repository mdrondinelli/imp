COMPILE_COMP = glslc -fshader-stage=comp -O --target-env=vulkan1.1
COMPILE_VERT = glslc -fshader-stage=vert -O --target-env=vulkan1.1
COMPILE_FRAG = glslc -fshader-stage=frag -O --target-env=vulkan1.1

all: scattering.spv optical_depth.spv atmosphere-vert.spv atmosphere-frag.spv

scattering.spv: scattering.glsl
	$(COMPILE_COMP) -o scattering.spv scattering.glsl

optical_depth.spv: optical_depth.glsl
	$(COMPILE_COMP) -o optical_depth.spv optical_depth.glsl

atmosphere-vert.spv: atmosphere-vert.glsl
	$(COMPILE_VERT) -o atmosphere-vert.spv atmosphere-vert.glsl

atmosphere-frag.spv: atmosphere-frag.glsl numeric.glsl
	$(COMPILE_FRAG) -o atmosphere-frag.spv atmosphere-frag.glsl
