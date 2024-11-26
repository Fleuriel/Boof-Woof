#pragma once
#ifndef PCH_H
#define PCH_H

// PRE-COMPILED HEADERS

// BASIC
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include <unordered_set>
#include <set>

#include <unordered_map>
#include <map>

#include <queue>
#include <array>
#include <bitset>
#include <vector>
#include <algorithm>
#include <memory>

#include <cstdint>
#include <cassert>

#include <functional>
#include <filesystem>
#include <random>
#include <chrono>
#include <typeindex>

// OPENGL & GLM
// Prevent GLFW from redefining APIENTRY
#ifdef APIENTRY
#undef APIENTRY
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 5000;

// A simple type alias
using ComponentType = std::uint32_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;

#endif