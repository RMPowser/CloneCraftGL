# Overview

CloneCraftGL is a complete rewrite of the original CloneCraft that uses OpenGL instead of Vulkan.

CloneCraft is a very simple Minecraft clone for Windows that I wrote for my midterm project over the course of one month. It's written in C/C++ and currently uses the OpenGL graphics API along with a few other quality-of-life libraries.

When I started the project, I had zero experience with any form of computer graphics, let alone Vulkan/OpenGL. Long story short, Vulkan is not a beginners API. It was hard, but I learned a lot in one month.

The original version of the game included many unfinished aspects such as the world only being one block thick, having only one block type (technically 2. air and grass), collision detection not working in negative coordinate space, no frustum culling, and it was even possible to crash your graphics driver. Most of these problems have since been fixed, but not all of them. 

## Features
- 3D graphics with OpenGL
- instanced rendering
- KB/M controls
- .OBJ file loading
- texture mapping
- collision detection
- infinite procedural terrain generation
- basic mining and building
- flight mode to turn off gravity

## Controls

`WASD	- move and strafe
Mouse	- look around
Space	- jump/fly up
Ctrl	- fly down
F	- toggle flying
Shift	- sprint`

## HOW TO BUILD

If you want the absolute most current version of the source, clone the `development` branch. The `main` branch is usually pretty out of date and only updated for new releases.

After cloning the repo, you'll need the boost library for C++ before you can build project files. Get it [here](https://www.boost.org/users/download/) and create a system environment variable called `BOOST_SDK`. Mine looks like this: `C:\SDKs\boost_1_73_0`.

Once you're done with that, open a terminal in the root of the repo. Run premake5 and pass it the name of the IDE you want to generate project files for. A list can be found [here](https://github.com/premake/premake-core/wiki/Using-Premake). I use Visual Studio 2019, so mine looks like this: `premake5 vs2019`.
	
Premake will create project files in the current directory and should handle includes and linking automatically. You should now be able to open the project files and then build and run the project.
