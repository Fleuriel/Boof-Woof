 ===========================================
		Milestone 2
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


 BUILD INSTRUCTIONS ======================

>  Open BoofWoof.sln, clean and rebuild
>  Go to ScriptWoof.sln, clean and rebuild
>  Under the lib folder, go to JoltPhysics-master > LinkLib, extract the zip files for the Jolt.lib
>  Set either Game or EditorPaws as your startup project, depending on what you are aiming to test.

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
 > R - Scent Trail
 > C - Catch Bone Catcher
 > Left Shift - Sprint 
 
To collect Dog bone & tennis ball, just walk towards it and it will be collected [for now].
To start Rope breaker, walk and collide with the rope [for now].
To exit game, please exit out of fullscreen and press close application [for now].

 KNOWN PROBLEMS & WILL CHANGE IN M4 ========================== 

>   Able to save archetypes & delete archetypes during runtime.
>   Will make dog footsteps sound different on different floors [different surface, different sound]
>   Editor cannot make changes in fullscreen for now.
>   Unable to exit game via menu [look at game controls section on how to exit].