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
#include <thread>

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstdarg>

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
#include <glm/vec2.hpp>
#include <glm/gtc/quaternion.hpp>   // For glm::quat
#include <glm/gtx/euler_angles.hpp> // For Euler angles

// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 5000;

// Define an Invalid Entity (use the maximum possible value for uint32_t)
constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

// A simple type alias
using ComponentType = std::uint32_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;

#endif