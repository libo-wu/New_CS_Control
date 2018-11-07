# New_CS_Control
Control program to run the new CS platform.  
Name: new thermopile sensor compressive sensing   
Author: Libo Wu, (libo.wu@tamu.edu, libo.wu1992@gmail.com)  

# General intuition
Compressive sensing is widely used in image processing/transmission, which is using linear measurements of the original sparse signal to reconstruct origianl signal with low error.  
This project is using a single thermopile sensor to perform indoor human localization by using a coded mask.  

# About this repository
This repository mainly contain the control program of Arduino Nano. These functions are called by entering BT command. The functions it contains are:  
1. create interrupt of detecting a optical endstop to find the starting position of the mask.
2. rotating the mask while aqcuire data from MLX90614.
