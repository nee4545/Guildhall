In the build I was mainly focused on making occupancy maps work.

To see this working:

-Click on play in the menu
-Press 5 to go to occupancy map game
-The top part of the map is single AI using single occupancy map
-The bottom part of the map is AI sharing occupancy map 
-Use WASD to move the player
-Press F to start simulating the game
-Press P to pause the game

Single AI using single occupancy map behavior
-If AI can see you it will follow you
-If it loses track of u occupancy map will be created to predict position

Shared Occupancy map AI behavior
-If any of the AI can see you it will follow you
-If any of the AI loses sight of you it will use occupancy maps and all the AI will try to find you 



