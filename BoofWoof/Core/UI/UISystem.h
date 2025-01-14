/**************************************************************************
 * @file UI.h
 * @author 	Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the UISystem class, which is used to
 * render UI components on the screen.
 * 
 *************************************************************************/
#pragma once
#ifndef UISYSTEM_H
#define UISYSTEM_H

#include <ft2build.h>
#include "../Graphics/Shader.h"
#include "../ECS/System.hpp"
#include FT_FREETYPE_H



//UI system
class UISystem : public System {
public:
	void UI_init();
	void UI_update();
	void UI_render();



private:
	Entity mSelectedEntity;
};
extern UISystem uiSystem;
#endif
