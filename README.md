# 2D Infinite Runner Game

## **Description**
This project is a 2D infinite runner side-view game built using OpenGL. The main objective is to dodge obstacles, collect items, and utilize power-ups within a limited game time. The game increases in difficulty as time progresses.

## **Game Features**

### **Gameplay**
- **Player Actions**: Jump and duck to dodge obstacles and collect items.
- **Health System**: Starts with 5 lives, represented visually (not numerically). Loses a life upon collision with obstacles.
- **Score System**: Increases as collectables are gathered.
- **Game Speed**: Increases over time, making the game more challenging.
- **Game End Conditions**: 
  - **All lives lost**: Displays "Game Lose."
  - **Time runs out**: Displays "Game End."

### **Models**
- **Player**: Composed of at least 4 primitives.
- **Obstacles**: Designed using 2 primitives, appear continuously at varying heights.
- **Collectables**: Created using 3 primitives, spawn infinitely.
- **Power-ups**: Two distinct types, designed with 4 primitives each, appear at least twice per game.

## **Technical Details**
- **Framework**: OpenGL
- **Models**: Designed using basic primitives (point, line, triangle, quad, polygon).
- **Animation**: Object movements and background elements.
- **Interactivity**: Controlled using keyboard inputs.

## **How to Run**
1. Ensure you have an environment set up for running OpenGL programs.
2. Compile the `P25-55-0406.cpp` file using your preferred compiler.
3. Run the executable to start the game.

## **Acknowledgment**
This project is developed as an individual assignment for DMET 502: Computer Graphics during Winter 2024 at the German University in Cairo.

