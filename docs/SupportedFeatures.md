# Supported features

This pages lists some of the features that Render Engine supports

## Table of contents

* [Render Paths](#render-paths)
* [Render Passes](#render-passes)
  * [Render pass](#render-pass)
  * [Shadow Caster pass](#shadow-caster-pass)
  * [Skybox pass](#skybox-pass)
  * [Gizmos pass](#gizmos-pass)
* [Lighting](#lighting)
* [Light sources](#light-sources)
  * [Directional light](#directional-light)
  * [Point light](#point-light)
  * [Spot light](#spot-light)
* [Instancing](#instancing)
* [Shader features](#shader-features)
* [Editor GUI](#editor-gui)
  * [Scene Hierarchy window](#scene-hierarchy-window)
  * [Debug Console window](#debug-console-window)

## Render Paths
Only **Forward** rendering path is implemented for now. **Forward+** and **Deferred** rendering paths support is planned as well.

## Render Passes

### Render pass

Default render pass that draws all renderers in the scene. It allows selecting sorting mode, filtering mode and clear flags.

It is used to render opaque and transparent gameobjects in two different passes.

### Shadow Caster pass

Pass that draws shadows for all light sources. It also sets up all the data and textures that will be supplied to the shader

### Skybox pass

Pass that draws cubemap skybox. It is executed between opaque and transparent passes.

### Gizmos pass

Pass that draws gizmos after all rendering is done. Gizmos are enabled with a `Debug/Draw Gizmos` menu.
It also draws outline for gameobjects that are selected in Scene Hierarchy window, regardless if gizmos are enabled or not.

By default, when gizmos enabled - AABBs of all renderers in the scene are drawn as wire boxes

## Lighting
* PBR with Cook-Torrance BRDF
* Blinn-Phong lighting model

## Light sources

Light sources' data is updated once per frame and supplied to shader with constant buffer,
as well as shadow casters data.

### Directional light

One directional light is supported. It can cast shadows with orthogonal projection - the scene is drawn into 2D texture.

### Point light

Several point lights are supported. Number of point lights is defined by the constant [GlobalConstants::MaxPointLightSources](../core/global_constants.h).

Point lights also support casing shadows - each light draws the scene with perspective projection in 6 directions, each time using different texture array slices for target.
Correct slice to sample is selected in shader based on max component of light-to-fragment vector (the same logic as hardware cubemap face selection).

### Spot light

Several spot lights are supported. Number of spot lights is defined by the constant [GlobalConstants::MaxSpotLightSources](../core/global_constants.h).

Spot light also support casting shadows - each light draws the scene with perspective projection to a separate slice of texture array.

## Instancing

Engine supports instanced draw calls. Shader that is wants to use instancing must be created with `_INSTANCING` keyword.

Engine automatically determines which gameobject are eligible for instanced draw and sets up all necessary data and rendering state.
Max number of gameobjects in a single instanced draw call is determined by [GlobalConstants::MaxInstancingCount](../core/global_constants.h)

Engine also supports having per-instance data for instanced draw call - each instance can have unique data. 
See [Standard](../resources/shaders/standard/standard_shared.cg) shader for an example of how to define per-instance data struct. 
After per-instance data is defined, it can be set using [Renderer](../core/renderer/renderer.h) API.
Engine will automatically copy all per-instance data from individual renderer's buffer to a single large buffer before issuing a draw call.

Currently, there are two implementations of how the data can be passed to an instanced draw call:
* Matrices and Per-Instance data are passed with SSBO (buffers that are basically unlimited in size). This implementation is used when Graphics Backend implementation supports SSBOs
* Matrices are passed as vertex attributes and Per-Instance data is passed with constant buffer. This implementation is used when Graphics Backend does not support SSBOs (OpenGL 4.1 on MacOS)

## Shader features
* Albedo textures
* Normal maps
* Multipass shaders based on tags
* Custom shader preprocessor - allows using `#include` instructions with GLSL
* Shader description in JSON - allows specifying passes, drawing parameters and default values for uniforms
* Alpha blending and alpha clipping

## Editor GUI

### Scene Hierarchy window

This window can be opened with `Windows/Scene Hierarchy`. It draws all gameObjects that are present in the scene in a hierarchical structure. Gameobjects can be reparented, renamed or deleted in this window.

### Debug Console window

This window can be opened with `Windows/Debug Console`. It shows all messages printed with `Debug::LogError`, that are also shown in console window in IDE. 
This window is opened automatically when log is issued.