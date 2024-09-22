#pragma once
#include <cstdint>
#include <unordered_set>
#include <queue>
#include <array>
#include <cassert>
#include <unordered_map>
#include <memory>
#include <bitset>
#include <iostream>

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