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
See "How to use the custom function mode"
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

## How To Use The Custom Function Mode
The custom function mode is a way for you to create your own equations. 
I wanted to represent this as a graph system but that was out of scope for this project.
I also considered writing a parser but I also believe that for it to be as feature rich
as I wanted it would also be out of scope. I settled with a pattern that is closely modeled
after the way the custom functions are modeled in code, for both feature completion and
speed of development.
##### How It Works
Custom functions are run using a series of commands. The arguments for a command
can be either the result of the previous iteration, the input position
(passed in through the x, y position of the pixel), the result of a previous
command, or a constant value (either a float or a complex float).
There is a custom function for both the starting value, and the recursive function.
The last command in the sequence is the "return value" of the custom function.
Most of the math functions provided are self explanatory math functions, but it's worth
noting the "move" function; this takes an input and simply sets the output of that
command as that input. This is useful in the starting function when you want the result
to be a constant or the input position.

For the fractal to be able to generate, you need at least one command for both the starting
function and the recursive function. If you use any constant variables, you will have to create a constant
variable using the + button for that data type. Constants can be set to specific pixel location values by 
clicking the input field for one,
holding "v", and mousing over the drawing window. Doing this will redraw the fractal.

#### Getting Started and Useful Setups
If you want to start messing around, the following patterns are good for creating fractals:
##### Simple Tower Functions
 Setting the starting function to "move" with the source being "input position", and then experimenting 
with different functions in the recursive function is a good starting point. There are a handful of interesting
fractals that can be made using this setup and just one command for the recursive function. Make sure your recursive
function command involves (n-1) as one of its inputs; you need this to get the recursive nature
of the fractal generation to work.
##### The Mandelbrot-like Setup
Mandelbrots use a starting position of (0,0) and use an "add" command at the end
of the recursive function to add the input position. Using the "move" command in your
starting function and setting it to move a constant complex float (make sure to add one to your
list of complex floats), and then adding a command to the recursive function with (n-1)
as an input, followed by an "add" command with one source being the previous command (make sure to
set the index to that of the command you want to use the result from) and the other source being "input position".
There are quite a few interesting results that can come from this.
##### The Julia-like Setup
For most fractals that you can get from the above Mandelbrot-like setup, there is
an associated "Julia set" like fractal. In order to make it, instead of having the starting function
be a constant, change the input of the "move" command to be the input position, and
instead of adding the input position at the end of the recursive function, add a constant complex float.
You will get a fractal that is similar to the "mandelbrot-like" version in some ways; setting the complex float
added in the recursive function to various values will change the result.
#### Tweaking The Gradient Using The "Power" Value
The "Power" value does not affect the actual equation of the fractal, but it affects
the way the gradient between iteration steps is generated. If you have an overly complex gradient for some fractals with
a higher "Power" value than 1 (usually tower functions are a good example that can cause this)
it's a good idea to set this value to 1. If you have a polynomial growth rate (depending on how you are doing
the math in your recursive function) you should tweak the power value to match the growth rate of your recursive function.
If you're unsure of how to know what power your growth rate is at, just try tweaking this value up and down until you get a smooth
gradient between iteration steps, or you get a more aesthetically pleasing result.