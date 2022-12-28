# Introduction
This repository contains a standalone version of the pong game made in C++ using SFML.
To play the game just by itself, set STREAM to be false before compiling.

In addition, SFML TCP sockets are also used for the game to communicate the state of the game with a python script 
in real time. Currently, the python script transmits random instructions. My aim is to train a Reinforcement Learning
agent that will be able to successfully play the game.

# C++ Compilation and Running (Linux)

- SFML can be installed using the command: sudo apt-get install libsfml-dev
- To Compile the program cd to the c_plus_plus_pong folder and use the command make
- The program can be run by typing make run
# Game Instructions

W/S - Left Slider up and down

Up/Down Arrows Right Slider up and down

8, 2, 4, 6 Control Ball to move up, down, left and right

P - Pause the game (game pauses by itself when a goal is scored)

C - Continue the game