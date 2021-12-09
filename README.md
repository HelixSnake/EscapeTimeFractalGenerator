# Escape Time Fractal Renderer
Practice project for C++. Designed to render escape time fractals on the CPU.
## Introduction
This was a personal project I made to practice C++. Fractals have always fascinate me, how incredibly complex patterns emerge from simple inputs. This project was designed both as a fun toy for me to play with, a practice ground for C++, and good content for my programming blog.

## Technologies:
GLEW  
GLFW3  
GLM  
Dear ImGui  
SOIL  
IMFileBrowser  

## Installation Instructions
In order to make this project work, you'll need to populate the lib/ directory with the submodules. To do this, run the following command in the base folder:
`git submodule update --init --recursive`
Currently this project is only designed to be ran on Windows.

## How To Use
##### Starting Out
You will need a file called "colorRamp.png" in the same folder as the executable. Otherwise you will not have a working render until you choose a color ramp file. Sample color ramps are supplied in the root folder.  

![DefaultUI](https://github.com/HelixSnake/EscapeTimeFractalGenerator/blob/main/Documentation/UI_Normal_View.png)  
You can see a set of controls here. Here are the explanations for them:
##### Iterations:
Determines the amount of recursive iterations the fractal goes through. More iterations means more expensive, but more accurate. Each pixel does not always run the full number of iterations, so rendering time does not always scale linearly with this value. If parts of the fractal are rendering black that you think should be filled in, increase this. Click "Update" after changing.
##### Length Limit
Determines the magnitude where the code "escapes" when the length of the recursive value is higher. For some fractals you may not see a difference but others (like the Peacock Fractal or the Cosine Fractal) may see improvements or changes with this value. Typically does not affect performance much. Click "Update" after changing.
##### Upsample
Determines the rendered texture resolution. Choose values between 0 and 1 to render at lower resolutions, and choose 2 or 4 to render at a higher resolution and downscale for better quality. Performance generally scales with the square of this value. Click "Update" after changing.
##### Period
Controls how much the ramp texture repeats. Only affects the "intrepretation" step, so this can be changed without clicking "Update" or re-rendering the fractal.
##### Offset
Offsets the ramp texture by a value between 0 and 1 for tweaking the visuals. Only affects the "intrepretation" step, so this can be changed without clicking "Update" or re-rendering the fractal.
##### Custom Function
See "How to use the custom function mode" [Under Construction]
##### Fractal Type
A handful of different fractals. Mouse over these for helpful notes on how to get them to work.

##### Update
You will need to click this after you do anything that would affect the fractal itself rendering.
##### Reset Zoom
Resets the zooming to the starting values.
##### Choose Ramp Texture
Choose a PNG on your hard drive, and the program will load in the top row of pixels for the color ramp. A handful of sample color ramps are included in the root folder. Click "Default" with a file called "colorRamp.png" in the same folder as the executable to load that one.
##### Show Advanced Options
These options relate to the cycle detection that exits the recursive function early when it detects a convergence. It's best not to mess with these; the default values work great for almost all cases.
##### Live Rendering
Makes the fractal render step by step as it is generated. Recommended to have on to see the progress of the rendering.
##### Progress Bars
There are 2 steps to the rendering process: the generation of the fractal, and the interpretation step that turns a value into a color from the color ramp. As these processes are separate, they are tracked by separate progress bars.