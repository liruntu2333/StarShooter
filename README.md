# Star Shooter
A mini 3D action game about shooting stars in a endless cycling space, rendering implemented with D3D11.
Reflection light on objects is sampled on a cube map.
Point light shadow is rendered with dual paraboloid shadow mapping technique with quite well performance.
Whole scene has five point light shadow maps with a resolution of 1024 in total.

![alt text](https://github.com/liruntu2333/StarShooter/blob/master/ScreenShot.png)

## Platform:

Any Windows device that can run with HLSL shader model 5.0.

Not sure what it takes to render with 5 DPSMs and 8x MSAA, let's assume that it would be 60 FPS+ if you have a GTX graphic card.

## Build & Runtime Requirement:

For some reason, we use the legacy DirectX End-User Runtimes (June 2010) lib for creating textures from file, compiling shaders.

https://www.microsoft.com/en-us/download/details.aspx?id=8109

Need to be installed in path : C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\...

## How to Play:

W A D : side-move and choose branch

←↑↓→ : shooting command

Left-shift : lock on target

SPACE : jump
