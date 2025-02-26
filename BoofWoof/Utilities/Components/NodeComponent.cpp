#include "pch.h"
#include "NodeComponent.hpp"

// Define the static variable
uint32_t NodeComponent::nodeCounter = 0;

// Constructor: Does NOT increment the counter automatically
NodeComponent::NodeComponent() {
    nodeID = INVALID_ENTITY;  // Default to invalid until assigned
    //std::cout << "[DEBUG] NodeComponent created. No ID assigned yet.\n";
}
