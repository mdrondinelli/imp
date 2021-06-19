COMPILE_COMP = glslc -fshader-stage=comp -O --target-env=vulkan1.1
COMPILE_VERT = glslc -fshader-stage=vert -O --target-env=vulkan1.1
COMPILE_FRAG = glslc -fshader-stage=frag -O --target-env=vulkan1.1

all: Transmittance.spv SkyView.spv AerialPerspective.spv AtmosphereVert.spv AtmosphereFrag.spv

Transmittance.spv: Transmittance.glsl Constants.glsl
	$(COMPILE_COMP) -o Transmittance.spv Transmittance.glsl

SkyView.spv: SkyView.glsl Constants.glsl
	$(COMPILE_COMP) -o SkyView.spv SkyView.glsl

AerialPerspective.spv: AerialPerspective.glsl Constants.glsl
	$(COMPILE_COMP) -o AerialPerspective.spv AerialPerspective.glsl

AtmosphereVert.spv: AtmosphereVert.glsl
	$(COMPILE_VERT) -o AtmosphereVert.spv AtmosphereVert.glsl

AtmosphereFrag.spv: AtmosphereFrag.glsl Constants.glsl Numeric.glsl
	$(COMPILE_FRAG) -o AtmosphereFrag.spv AtmosphereFrag.glsl
