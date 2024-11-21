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

// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str);

// Function to process the descriptor file and print details
std::vector<std::string> processTextureDescriptorFile(const std::string& descriptorFilePath);

int runCommand(const std::string& command);

int test();
#endif // TEXTURE_COMPRESSONATOR_H
