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

 KNOWN PROBLEMS & WILL CHANGE IN M3 ========================== 

>   To control the lighting in the scene, ensure that the game object have a graphics component and you will be able
to change the lighting as needed. Uncheck the LightOn to see the whole assets in full glory.
>   Allowing designers to change the collision components (physics).
>   Able to delete and look at the list of materials attached to the object.
>   Able to save archetypes & delete archetypes during runtime.
>   Able to move camera in Game.exe