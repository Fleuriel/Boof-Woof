#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Graphics/Mesh.h"
#include "../Core/Graphics/Shader.h"

#define PARTICLE_NUM 100

class ParticleComponent
{
public:
	ParticleComponent() {};
	~ParticleComponent() {
		particles.clear();
	};

	class Particle
	{
	public:
		Particle(glm::vec3* pos)
			: position(pos)
		{}
		~Particle() {};

		void update(float dt)
		{
			*position += velocity * dt;
			lifeCount += dt;
		}
		
		glm::vec3* position{};
		glm::vec3 velocity{};
		glm::vec3 direction{};
		float lifeTime{};
		float lifeCount{};
	};

	void init( )
	{
		particles.reserve(PARTICLE_NUM);
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			translation[i] = glm::vec3(0.0f, 0.0f, 0.0f);
			auto ptc = std::make_unique<Particle>(&translation[i]);
			ptc->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			ptc->direction = glm::vec3(0.0f, 0.0f, 0.0f);
			ptc->lifeTime = 0.0f;
			ptc->lifeCount = 0.0f;
			particles.push_back(std::move(ptc));
		}

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_mesh.vertices), &particle_mesh.vertices, GL_STATIC_DRAW);
		// vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



		// instance VBO
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(2, 1);
		glBindVertexArray(0);


	}

	void update(float dt)
	{
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			particles[i]->update(dt);
		}
	}

	void draw(OpenGLShader shader)
	{
		shader.Use();
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, PARTICLE_NUM);
		glBindVertexArray(0);
		shader.UnUse();
	}
	


	REFLECT_COMPONENT(ParticleComponent)
	{

	}


private:
	std::vector<std::unique_ptr<Particle>> particles{};
	glm::vec3 translation[PARTICLE_NUM]{};
	GLuint instanceVBO{}, quadVAO{}, quadVBO{};
	Mesh particle_mesh{};

	
};



#endif