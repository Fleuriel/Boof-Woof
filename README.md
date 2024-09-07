Assignment 2 - Bounding Volumes & BVH

Angus Tan Yit Hoe
tan.a@digipen.edu


================Table of Contents================
i. PROJECT DESCRIPTION
A. HOW TO USE
B. ASSUMPTIONS
C. COMPLETED
D. NOT COMPLETED
E. SOURCE CODE LOCATION
F. MACHINE TESTED ON
G. HOURS SPENT ON ASSIGNMENT
H. ANY OTHER INFORMATION



====================================================================================
===============================PROJECT DESCRIPTION==================================
====================================================================================
The purpose of this assignment is to focus on building bounding volume hierarchy
framework for the CS350 Class.


NOTE: Coordinates are in 3D, and are scaled up to 100 * speed for camera speed,
whereby speed is 5 if pressed LSHIFT, and 1 if not pressed. 

--> Camera Movement is done based on XZ- Axis/Coords: WASD
--> Camera Movement is done based on Y- Axis/Coords:  Z to move up, X to move down
--> Camera is Perspective
====================================================================================
=====================================HOW TO USE=====================================
====================================================================================
2 Menu, Left and Right Menu docked to the left and right of the screen.

Left Menu consist of the radio buttons to see the bounding volumes.

Right Menu is to change the scale, position and angle of the primitives on the
screen.



************************************************************************************
WHERE TO PUT MODEL FILE
************************************************************************************

PowerPlant Model used: PPSECTION5

../Adv Computer Graphics/Model/PPlantModel

The Model Directory SHOULD be:


Directory: ../Adv Computer Graphics/Model/PPlantModel
/ppsection5
Section5.txt


Directory: ../Adv Computer Graphics/Model/PPlantModel/ppsection5
/parta
/partb
/partc


Directory:
 ../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_a
 ../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_b
 ../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_c

Each of the directory should have
g0.obj ... ... ... 

Total of 40 obj files.


====================================================================================
====================================ASSUMPTIONS=====================================
====================================================================================
Program might crash and/or have errors if not in focus. i.e. Alt Tab

====================================================================================
======================================COMPLETED=====================================
====================================================================================

====================================================================================
SCENE SETUP: 
MODEL LOADING
====================================================================================


====================================================================================
BOUNDING VOLUME:

AABB
SPHERE

? RITTER METHOD
? MODIFIED LARSSON'S METHOD
? PCA METHOD
====================================================================================
====================================================================================
INTERACTIVITY/ DISPLAY:

TOGGLE BOUNDING VOLUMES,
DISPLAY BOUNDING VOLUMES IN WIREFRAME (IMGUI TOP LEFT)
CAMERA IMPLEMENTATION (PARTIALLY WITH MOUSE)
====================================================================================



====================================================================================
====================================NOT COMPLETED?==================================
====================================================================================

Bounding Volume Hierarchy 
? RITTER METHOD
? MODIFIED LARSSON'S METHOD
? PCA METHOD




====================================================================================
===============================SOURCE CODE LOCATION=================================
====================================================================================

From Solution Directory:

SOURCE CODE
Adv Computer Graphics\Source

BoundingVolume.cpp 		(Bounding Volumes)
Graphics.cpp 			(Graphics)
GraphicsUserInterface.cpp 	(ImGUI)


HEADER CODES
Adv Computer Graphics\Headers

BoundingVolume.h
Graphics.h
GraphicsUserInterface.h


SHADER FILES
Adv Computer Graphics\Shaders

Shader.frag
Shader.vert

====================================================================================
================================MACHINE TESTED ON===================================
====================================================================================

DIT2535US (EDISON K ROW)


====================================================================================
============================HOURS SPENT ON ASSIGNMENT===============================
====================================================================================

50 - 70 HOURS PER WEEK. (MOSTLY ON RENDERING MODEL)

====================================================================================
==============================ANY OTHER INFORMATION=================================
====================================================================================


