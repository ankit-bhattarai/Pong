# Introduction
This repository contains a standalone version of the pong game made in C++ using SFML.

In addition, TCP sockets are also used for the game to communicate the state of the game with a python script 
in real time. Currently, the python code has a function can exactly determine the best place for the sliders to be to hit the ball. 
My aim is to train a Reinforcement Learning agent that will be able to successfully play the game.

If wanting to play the game by itself, set left_slider_manual or right_slider_manual to true to be able to control those sliders.

# C++ Compilation and Running (Linux)

- SFML can be installed using the command: sudo apt-get install libsfml-dev
- To Compile the program cd to the c_plus_plus_pong folder and use the command: make
- The program can be run with the command: make run

# Running Python Server for controlling the game 
- After compiling the C++ Program but before running it, run the python file server.py
- Once the script has started running, run the compiled C++ game

# Game Instructions

W/S - Left Slider up and down

Up/Down Arrows Right Slider up and down

8, 2, 4, 6 Control Ball to move up, down, left and right

P - Pause the game (game pauses by itself when a goal is scored)

C - Continue the game
