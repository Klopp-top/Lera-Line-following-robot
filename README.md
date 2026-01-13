# Line Follower Robot (PD Controller)

This project is a line follower robot that uses **8 independent line sensors** and a **PD controller without an integral (I) term**.

## Description
The robot is designed to follow a line with high accuracy and fast response.  
A PD control algorithm is used to correct deviations from the line without using the integral component, which helps avoid error accumulation and oscillations.

## Features
- 8 separate line sensors  
- PD control (Proportional + Derivative)  
- No integral term (I = 0)  
- Fast response to sharp turns  
- Stable behavior at high speed  

## Working Principle
1. Sensors detect the line position  
2. The error relative to the center is calculated  
3. PD controller computes correction:
   - P term handles current error  
   - D term reacts to changes in error  
4. Motor speeds are adjusted to stay on the line  

## Control Formula
Output = Kp * error + Kd * (error - previous_error)

## Applications
- Line follower competitions  
- Robotics and control system learning  
- PD controller tuning experiments  

## Project Structure
- `/unreal_line_foolower` — source code  
- `/docs` — documentation  
- `/hardware` — sensors and wiring  

## Status
Project is under development.
