# Renderer Engine

A small C++ OpenGL renderer/editor scaffold in `Renderer_Opengl/Engine`.

## Screenshots

![Renderer Engine Editor - First View](Screenshots/1.png)

![Renderer Engine Editor - Viewport and Gizmos](Screenshots/2.png)
## What Is Added

- OpenGL renderer/editor scaffold
- Dear ImGui editor UI
- Framebuffer viewport inside the editor
- Scene Hierarchy and Inspector panels
- Editable cube objects
- Selected-object outline
- Sky-colored viewport background
- Ground grid
- Move, rotate, and scale tool guides
- Mouse camera navigation
- Mouse-based object movement, rotation, and scale
- Material color controls
- Scene save/load
- Console/log panel
- Lighting controls
## Editor Features

The editor has a basic Dear ImGui layout:

- `File` menu
- `View` menu
- `Toolbar`
- `Scene Hierarchy`
- `Viewport`
- `Inspector`
- `Stats`
- `Lighting`
- `Console`
- `Controls`

The `Scene Hierarchy` lets you select objects and add/delete cubes.

The `Inspector` lets you edit the selected cube name, position, rotation, scale, and material color.

The `Viewport` renders the OpenGL scene into a framebuffer texture and displays it inside ImGui.

## Viewport Controls

- Hold right mouse and move mouse: look around scene camera
- Hold right mouse + `W/A/S/D`: fly camera
- Hold right mouse + `Space/C`: move camera up/down
- Middle mouse drag: pan scene
- Mouse wheel: zoom camera

## Toolbar Tools

- `Select`: left mouse drag pans scene
- `Move`: left mouse drag moves selected object on X/Y
- `Move`: mouse wheel moves selected object on Z depth
- `Rotate`: left mouse drag rotates selected object on X/Y
- `Rotate`: mouse wheel rotates selected object on Z roll
- `Scale`: left mouse drag scales selected object
- `Camera`: viewport camera navigation mode

## Visual Editor Helpers

- Yellow outline shows the selected object
- Move tool shows red/green/blue axis directions
- Rotate tool shows a circular guide
- Scale tool shows scale handles
- Viewport uses a sky-colored background and ground grid

## Build

From this folder:

```powershell
cmake -S . -B build
cmake --build build
```

Run:

```powershell
.\build\Sandbox.exe
```

## Required Libraries

Already connected locally or through MSYS2:

- OpenGL
- GLFW
- GLEW
- Dear ImGui

Local library folder:

```text
Renderer_Opengl/Libraries/
  glew-2.3.1/
  glfw-3.4.bin.WIN64/
  imgui/
```

## Feature Status

- Done: material color system
- Done: scene save/load through `Scenes/default.scene`
- Done: console/log panel
- Done: lighting controls
- Added groundwork: texture class and checkerboard texture support
- Blocked by dependency: full image texture loading needs `stb_image`
- Blocked by dependency: model loading needs Assimp installed, for example `pacman -S mingw-w64-ucrt-x86_64-assimp`
- Next: real 3D transform gizmo picking




