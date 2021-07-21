COMPILE_COMP = glslc -fshader-stage=comp -O --target-env=vulkan1.1
COMPILE_VERT = glslc -fshader-stage=vert -O --target-env=vulkan1.1
COMPILE_FRAG = glslc -fshader-stage=frag -O --target-env=vulkan1.1

all: Transmittance.spv SkyView.spv Render.spv CompositeVert.spv CompositeFrag.spv

Transmittance.spv: Transmittance.glsl Constants.glsl Scene.glsl
	$(COMPILE_COMP) -o Transmittance.spv Transmittance.glsl

SkyView.spv: SkyView.glsl Constants.glsl Intersections.glsl Scene.glsl SceneView.glsl
	$(COMPILE_COMP) -o SkyView.spv SkyView.glsl

Render.spv: Render.glsl Constants.glsl Intersections.glsl Numeric.glsl Scene.glsl SceneView.glsl
	$(COMPILE_COMP) -o Render.spv Render.glsl

CompositeVert.spv: CompositeVert.glsl
	$(COMPILE_VERT) -o CompositeVert.spv CompositeVert.glsl

CompositeFrag.spv: CompositeFrag.glsl
	$(COMPILE_FRAG) -o CompositeFrag.spv CompositeFrag.glsl

AerialPerspective.spv: AerialPerspective.glsl Constants.glsl
	$(COMPILE_COMP) -o AerialPerspective.spv AerialPerspective.glsl

RenderVert.spv: RenderVert.glsl SceneView.glsl
	$(COMPILE_VERT) -o RenderVert.spv RenderVert.glsl

RenderFrag.spv: RenderFrag.glsl Constants.glsl Scene.glsl SceneView.glsl
	$(COMPILE_FRAG) -o RenderFrag.spv RenderFrag.glsl

