# Procedural Terrain Generation System

## Overview

This project is a procedural terrain generation system developed in C++ using OpenGL. It dynamically generates 3D terrains using the Perlin noise algorithm, showcasing real-time terrain modification and parallel programming techniques to optimize performance.

## Features

- **Noise-Based Terrain Generation:** Utilizes Perlin noise to generate realistic terrain features.
- **Multithreading:** Leverages C++ standard threading to parallelize terrain generation.
- **User Interface:** Integrates a GUI for real-time adjustments of terrain generation parameters.
- **Camera Controls:** Implements basic camera controls for navigating the generated terrain.

## Screenshots

![Island 2](https://github.com/IvanPeevski/terrain-generation/assets/37153196/ae20c77a-3ee2-4dd2-8fbf-c3de5c6202e3)

![Island 1](https://github.com/IvanPeevski/terrain-generation/assets/37153196/86fbc72e-0e6d-4e5d-86ed-b0f580f64a82)

![Island 3](https://github.com/IvanPeevski/terrain-generation/assets/37153196/e4b823eb-ac0a-43eb-b95d-418a74a493ef)

## Getting Started

### Prerequisites

- C++ Compiler (GCC or MSVC)
- OpenGL
- GLFW and GLEW (for window management and OpenGL function loading)
- ImGui (for the GUI)

### Building the Project

1. **Clone the repository:**

2. **Navigate to the project directory:**

cd ProceduralTerrainGenerationSystem

3. **Compile the project:**
- On Windows using Visual Studio, open the solution file and build the project.
- On Linux, run the makefile from the terminal:
  ```
  make
  ```

### Running the Application

- After building the project, run the executable generated in the `/bin` directory.
- Use the GUI to adjust terrain parameters and explore different terrain features.

## Usage

- **Adjusting Terrain Parameters:** Open the GUI panel to tweak noise scale, octaves, and other parameters to see their effect on the terrain in real-time.
- **Navigating the Terrain:** Use WASD keys for movement and the mouse to look around. Right-click to toggle the mouse cursor.

## Contributing

Contributions are welcome! If you have ideas for improving this project, please fork the repository and submit a pull request. For major changes, please open an issue first to discuss what you would like to change.