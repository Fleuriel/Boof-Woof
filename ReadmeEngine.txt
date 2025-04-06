 ===========================================
		Milestone 6
 ===========================================
BoofWoof consists of:

~ RTIS
 > Angus Tan Yit Hoe
  <tan.a@digipen.edu>
 > Aaron Chan Jun Xiang	
  <aaronjunxiang.chan@digipen.edu>		
 > Chen Guo	
  <g.chen@digipen.edu>		
 > Liu Xujie	
  <l.xujie@digipen.edu>

~ IMGD		
 > Ang Jun Sheng Aloysius
  <a.junshengaloysius@digipen.edu>		
 > Goh Rachelle
  <rachelle.goh@digipen.edu>			
 > Quek Cheng Kwang Emery
  <c.quek@digipen.edu>			
 > Luke Goh Weng Ee	
  <goh.l@digipen.edu>		
 > Tan Wei Fong		
  <weifong.tan@digipen.edu>	
 > Tan Wei Jie Clement	
  <weijieclement.tan@digipen.edu>		


We do not have BFAs or UXGDs.	

All art assets are drawn by Emery Quek Cheng Kwang and Luke Goh Weng Ee.
All audios taken from Audio Professors & Soundly by Tan Wei Fong.


 BUILD INSTRUCTIONS ======================

 >  Open BoofWoof.sln, clean and rebuild
 >  Go to ScriptWoof.sln, clean and rebuild
 >  Under the lib folder, go to JoltPhysics-master > LinkLib, extract the zip files for the Jolt.lib
 >  Under the lib folder, extract the assimp2016.zip file
 >  Set either PawsieAdventure or EditorPaws as your startup project, depending on what you are aiming to test.

If you have linker error for Jolt.lib, please unzip the folders in lib > JoltPhysics-master > LinkLib
If you have linker error for assimp.lib, please unzip the folders in lib > assimp2016
If you try to move using WASD keys and you cannot move the corgi, please clean and build ScriptWoof.

 EDITOR CONTROLS ========================== 

 >  W / A / S / D - Move Player
 >  Arrow Keys, Right Shift and Right Ctrl - Move Editor Camera

 GAME CONTROLS ============================

 > F11 - Fullscreen / Exit Fullscreen
 > W / A / S / D - Move Player
 > Spacebar - Jump
 > RMB - Bite (To bite Rope & Cages)
 > E - Scent Trail
 > Left Shift - Sprint 
 > LMB - To advance in dialogue 

- Warning, Rex Patrols and Chases you! 
- Find you Puppies in the Main Hall and break them out of the cages! 
- To break them out, you have to complete Cage Breaker Puzzle! Afterwards, walk towards the Puppies to collect them.
- Press E to sniff where Rex's urines are at. To wash it off, go towards the Water Bucket.
- To start Rope breaker, walk and collide with the rope AFTER puppies are all collected.
- After Drawbridge lands on the ground, our beautiful end cutscene will play!
- To exit game, press ESC and exit game to return to main menu and click on the X button on the top right.

 MAJOR CHANGES IN M6 ==========================

 > Added in 3D objects as particles
 > Added in more 3D animations - Puppies, Scruffy, Rex and Cage
 > Added in more Audio for cutscenes and menus
 > Added Sensor for collision that will not affect Player/Rex
 > Added Scent trails on the floor for Player to follow
 > Added VFX when timer is low and when Player touches toy
 > Expanded StartingRoom and Changed a room design in Main Hall
 > Added Master Volume in Settings and having persistent settings
 > Added scene fade out and in before going to another scene with scene name in Loading screen

 KNOWN BUG ==========================

 > If you're in RopeBreaker and Rex touches you, the scene will reset and you might not be able to play the game properly
 > If you set master volume to 3 and you touch the water bucket, the master volume will be resetted back to 10 in the game but when you open the settings, it goes back to 3