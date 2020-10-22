Controls:

Move the camer towards the edges of screen to move camera
Left click to move the player to the clicked position
Right click to place the potential field around the map

A* pathfinding - 4-way and 8-way tile movement

Debugging for pathfinding:

Press '2' to enter pathfinder mode
press '1' to enter game mode
Press '3' to enter map creation mode
Press '4' to enter potential field creator mode

Note: I actually don't want you to play with map creation mode and potential field creator mode becase its really
      for me to explain in readme the functionality of it
      Its actually used to paint solid tiles on map and create potential field for a turret


Pathfinder Mode:

-You can select the current algorithm from the IMGUI window
-Use left mouse button to select the start and end tile
-Once both tiles are set hold the right mouse button to genereate path or press 'B' for a single step
-Press space to reset and clear the path
-Press 'C' to generate influence maps, choose whethere it is positive or negative from the IMGUI window

Game Mode: 

-AI chases the player if the player is in the sight sector
-AI choses a random tile and moves towards that otherwise


Main Menu : Currenly supported buttons
Click on play to open the game in play mode
Click on Quit to quit the game
  


