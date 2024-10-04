#pragma once
#ifndef TEXTURE_COMPRESSONATOR_H
#define TEXTURE_COMPRESSONATOR_H

#include <string>
#include <vector>

// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str);

// Function to process the descriptor file and print details
std::vector<std::string> processDescriptorFile(const std::string& descriptorFilePath);

int runCommand(const std::string& command);

int test();
#endif // TEXTURE_COMPRESSONATOR_H
