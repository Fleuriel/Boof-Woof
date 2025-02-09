 ===========================================
		Milestone 4
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
 >  Set either PawsieAdventure or EditorPaws as your startup project, depending on what you are aiming to test.

If you have linker error for Jolt.lib, please unzip the folders in lib > JoltPhysics-master > LinkLib
If you do not see anything on the screen when you load a scene, please clean and build MeshCompiler.
If you try to move using WASD keys and you cannot move the corgi, please clean and build ScriptWoof.

 EDITOR CONTROLS ========================== 

 >  W / A / S / D - Move Player
 >  Arrow Keys, Right Shift and Right Ctrl - Move Editor Camera

 GAME CONTROLS ============================

 > F11 - Fullscreen / Exit Fullscreen
 > W / A / S / D - Move Player
 > Spacebar - Jump
 > RMB - Bark (Only needed in starting room for now)
 > E - Scent Trail
 > C - Catch Bone Catcher (Temporary)
 > Left Shift - Sprint 

Currently Rex is just patrolling. 
To collect Puppies, just walk towards it and it will be collected [for now].
To get the Rex's Urine Scent off you, go towards the Water Bucket.
To start Rope breaker, walk and collide with the rope AFTER puppies are all collected.
To exit game, press ESC and exit game to return to main menu and click on the X button on the top right.

 MAJOR CHANGES IN M4 ==========================

 > UI System
 > Font System
 > Spatial Audio

 KNOWN PROBLEMS & WILL CHANGE IN M5 ========================== 

>   