#pragma once
#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
//#include "../Core/AssetManager/AssetManager.h"
// why I cannnot include AssetManager.h here?????????????????????????????????????

#include "../Core/Graphics/Model.h"
#include "../Core/Graphics/Object.h"



class GraphicsComponent {
public:
    // add model
    void addModel(std::string const& path);

	// add object
	void addObject(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);

	// set position
	void setPosition(Object& object, glm::vec3 position) { object.position = position; }

	// set scale
	void setScale(Object& object, glm::vec3 scale) { object.scale = scale; }

	// set rotation
	void setRotation(Object& object, glm::vec3 rotation) { object.rotation = rotation; }

	// set color
	void setColor(Object& object, glm::vec3 color) { object.color = color; }

	// set model
	void setModel(Object& object, Model* model) { object.model = model; }



    // Constructor and destructor
    GraphicsComponent();
//    GraphicsComponent(const OpenGLModel& model, int id = 0);
    ~GraphicsComponent() = default;
};

extern std::list<GraphicsComponent> objects;