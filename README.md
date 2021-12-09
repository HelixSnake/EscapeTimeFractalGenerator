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
Determines the amount of recursive iterations the fractal goes through. 
More iterations means more expensive, but more accurate. 
Each pixel does not always run the full number of iterations, 
so rendering time does not always scale linearly with this value. 
If parts of the fractal are rendering black that you think should be filled in, 
increase this. _Click "Update" after changing._
##### Length Limit
Determines the magnitude where the code "escapes" when the length of the recursive value is higher. 
For some fractals you may not see a difference but others (like the Peacock Fractal or the Cosine Fractal) 
may see improvements or changes with this value. Typically does not affect performance much. _Click "Update" after changing._
##### Upsample
Determines the rendered texture resolution. Choose values between 0 and 1 to render at lower resolutions, 
and choose 2 or 4 to render at a higher resolution and downscale for better quality. 
Performance generally scales with the square of this value. _Click "Update" after changing._
##### Period
Controls how much the ramp texture repeats. _Only affects the "intrepretation" step, 
so this can be changed without clicking "Update" or re-rendering the fractal._
##### Offset
Offsets the ramp texture by a value between 0 and 1 for tweaking the visuals. 
_Only affects the "intrepretation" step, so this can be changed without clicking "Update" or re-rendering the fractal._
##### Custom Function
See "How to use the custom function mode" [Under Construction]
##### Fractal Type
A handful of different fractals. Mouse over these for helpful notes on how to get them to work. 
_Click "Update" after changing._
##### Fractal Power
Controls the power used by some fractal algorithms as well as the way the gradient is rendered.
For the Peacock and Cosine fractals, set this to 1.0 for the best visuals. For the Super Mandelbrot/Julia fractals, 
this should be the same as the "X" component of the Additional Constant (floating point values allowed). 
For everything else, choose an integer value of 2.0 or higher. _Click "Update" after changing._
##### Custom Julia Position
Use to control the constant used for Julia Sets. You can hand-set this value, 
or mouse over the drawing window and press/hold J to choose a Julia set for that value. 
Pressing J will re-render the fractal. _Click "Update" after changing._
##### Additional Constant
Use to control the constant used in the generation of some fractals. 
If the fractal you are rendering has one of these, the fields will show up automatically. 
You can hand-set this value, or mouse over the drawing window and press/hold V to choose a constant for that value. 
Pressing V will re-render the fractal. 
The Super Mandelbrot and Peacock Fractals require this to be set to something other than 0 in order to render.
_Click "Update" after changing._

##### Update
You will need to click this after you do anything that would affect the fractal itself rendering.
##### Reset Zoom
Resets the zooming to the starting values. _Will re-draw the fractal._
##### Choose Ramp Texture
Choose a PNG on your hard drive, and the program will load in the top row of pixels for the color ramp. 
A handful of sample color ramps are included in the root folder. 
Click "Default" with a file called "colorRamp.png" in the same folder as the executable to load that one. 
_Only affects the "intrepretation" step, so this can be changed without clicking "Update" or re-rendering the fractal._
##### Show Advanced Options
These options relate to the cycle detection that exits the recursive function early when it detects a convergence. 
It's best not to mess with these; the default values work great for almost all cases.
##### Live Rendering
Makes the fractal render step by step as it is generated. Recommended to have on to see the progress of the rendering.
##### Progress Bars
There are 2 steps to the rendering process: the generation of the fractal, 
and the interpretation step that turns a value into a color from the color ramp. 
As these processes are separate, they are tracked by separate progress bars.