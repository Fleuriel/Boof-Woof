/**************************************************************************
 * @file Compressonator.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file declares functions that uses compressonator
 *
 *************************************************************************/

#pragma once
#ifndef TEXTURE_COMPRESSONATOR_H
#define TEXTURE_COMPRESSONATOR_H


#include "Descriptor.h"

std::string trim(const std::string& str);

int CompressTextureWithDescriptor(const TextureDescriptor& descriptor, const std::string& inputTexturePath, const std::string& outputTexturePath);

int runCommand(const std::string& command);

#endif // TEXTURE_COMPRESSONATOR_H
