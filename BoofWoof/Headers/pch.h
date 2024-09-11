#pragma once
#include <array>
#include <memory>
#include <unordered_map>
#include <cassert>
#include <bitset>
#include <queue>
#include <cstdint>
#include <set>
#include <iostream>


// A simple type alias
using ComponentType = std::uint8_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 5000;

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;