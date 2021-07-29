COMPILE_COMP = glslc -fshader-stage=comp -O --target-env=vulkan1.1
COMPILE_VERT = glslc -fshader-stage=vert -O --target-env=vulkan1.1
COMPILE_FRAG = glslc -fshader-stage=frag -O --target-env=vulkan1.1

all: GenericVert.spv TransmittanceFrag.spv SkyViewFrag.spv PrimaryFrag.spv IdentityFrag.spv BlurFrag.spv BloomFrag.spv CompositeVert.spv CompositeFrag.spv

GenericVert.spv: GenericVert.glsl
	$(COMPILE_VERT) -o GenericVert.spv GenericVert.glsl

TransmittanceFrag.spv: TransmittanceFrag.glsl Constants.glsl Scene.glsl
	$(COMPILE_FRAG) -o TransmittanceFrag.spv TransmittanceFrag.glsl

SkyViewFrag.spv: SkyViewFrag.glsl Constants.glsl Intersections.glsl Scene.glsl SceneView.glsl
	$(COMPILE_FRAG) -o SkyViewFrag.spv SkyViewFrag.glsl

PrimaryFrag.spv: PrimaryFrag.glsl Constants.glsl Intersections.glsl Scene.glsl SceneView.glsl
	$(COMPILE_FRAG) -o PrimaryFrag.spv PrimaryFrag.glsl

IdentityFrag.spv: IdentityFrag.glsl
	$(COMPILE_FRAG) -o IdentityFrag.spv IdentityFrag.glsl

BlurFrag.spv: BlurFrag.glsl
	$(COMPILE_FRAG) -o BlurFrag.spv BlurFrag.glsl

BloomFrag.spv: BloomFrag.glsl
	$(COMPILE_FRAG) -o BloomFrag.spv BloomFrag.glsl

CompositeVert.spv: CompositeVert.glsl
	$(COMPILE_VERT) -o CompositeVert.spv CompositeVert.glsl

CompositeFrag.spv: CompositeFrag.glsl
	$(COMPILE_FRAG) -o CompositeFrag.spv CompositeFrag.glsl
