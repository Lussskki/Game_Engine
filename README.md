# Renderer Engine

A small C++ OpenGL renderer/editor scaffold in `Renderer_Opengl/Engine`.

## Screenshots

![Renderer Engine Editor - First View](Screenshots/1.png)

![Renderer Engine Editor - Viewport and Gizmos](Screenshots/2.png)

## What We Built

- CMake project setup
- GLFW window creation and OpenGL context
- GLEW OpenGL function loading
- Application main loop
- Shader loading from files
- Shader compile/link error logging
- VAO, VBO, and EBO wrapper classes
- Mesh class with cube geometry
- Transform system with position, rotation, and scale
- Scene system with editable cube objects
- Perspective camera
- Framebuffer-based editor viewport
- Dear ImGui editor interface
- Scene Hierarchy panel
- Inspector panel
- Stats panel
- Controls panel
- Toolbar with `Select`, `Move`, `Rotate`, `Scale`, and `Camera`
- Selected-object yellow outline
- Unity-like sky/grid viewport environment
- Move, rotate, and scale visual gizmo guides
- Mouse-based object editing
- Mouse-based scene navigation

## Editor Features

The editor has a basic Dear ImGui layout:

- `File` menu
- `View` menu
- `Toolbar`
- `Scene Hierarchy`
- `Viewport`
- `Inspector`
- `Stats`
- `Controls`

The `Scene Hierarchy` lets you select objects and add/delete cubes.

The `Inspector` lets you edit the selected cube name, position, rotation, and scale.

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

## Next Steps

- Add texture loading with `stb_image`
- Add material system
- Add model loading with Assimp
- Add scene save/load
- Add console/log panel
- Add asset browser
- Add lighting controls
- Add real 3D transform gizmo picking
