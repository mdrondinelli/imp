#version 450 core

#include "Intersections.glsl"
#include "Numeric.glsl"

#define SCENE_SET 0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET 0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3, rgba16f) restrict writeonly uniform image2D multipleScatteringLut;

const float STEPS = 20.0f;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {

}
