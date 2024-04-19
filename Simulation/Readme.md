# Robotic Vacuum Cleaner Simulation

Welcome to the simulation stage of our Robotic Vacuum Cleaner project! In this stage, we use Godot Engine to simulate the behaviour of our vacuum cleaner robot in various room layouts and test its performance in cleaning efficiency.

## Simulation Environment

Our simulation environment is built using Godot Engine, with a simple robot model implemented as a KinematicBody. The robot model is equipped with three sensor areas to capture environmental data for navigation and obstacle avoidance.

### Features:

- **Dynamic Room Layout:** We utilize Godot's GridMap feature to easily change the layout of the room, allowing us to test the vacuum cleaner's performance in different environments.
  
- **Sensor Functionality:** The robot's sensors simulate real-world detection capabilities, enabling it to perceive obstacles and navigate around them efficiently.
  
- **Customizable Parameters:** We provide options to adjust parameters such as room size, obstacle density, and sensor sensitivity to fine-tune the simulation according to specific scenarios.

## Comparing Cleaning Times

To evaluate the efficiency of our robotic vacuum cleaner, we first conducted simulations for different existing algorithms in three different room types:  Empty Even Room, Empty Uneven Room, and Room with Obstacles. Here's a comparison of the cleaning times in sec required to achieve 50% and 100% room cleanliness for each room type:

|                     | Empty Even Room | Empty Uneven Room  |Room with Obstacles| 
|---------------------|-----------------|--------------------|-------------------|
| S-Shape             | 73 (50%)        | 150 (50%)          | 80 (50%)          |                       
|                     | 110 (99%)       | 270 (80%)          | 240 (70%)         |               
| Circular            | 62 (50%)        | 62 (50%)           | 78 (50%)          |                   
|                     | 150 (99%)       | 160 (95%)          | 240 (75%)         |           
| Random              | 55 (50%)        | 50 (50%)           | 220 (50%)         |                 
|                      |240 (85%)       | 210 (85%)          | 300 (80%)         |
| Random-90           | 76 (50%)        | 70 (50%)           | 90 (50%)          |               
|                     | 234 (90%)       | 180 (75%)          | 270 (70%)         |


## How to Run the Simulation

1. **Install Godot Engine:** Download and install Godot Engine from the official website (https://godotengine.org/).
  
2. **Clone the Repository:** Clone or download this repository to your local machine.
  
3. **Open the Project:** Open the project folder in Godot Engine.
  
4. **Adjust Parameters:** Modify the simulation parameters as needed, such as room layout, obstacle density, and sensor settings.
  
5. **Run the Simulation:** Start the simulation to observe the behaviour of the robotic vacuum cleaner in the simulated environment.
  
6. **Analyze Results:** Analyze the simulation results to evaluate the performance of the vacuum cleaner and compare cleaning times across different room types.


