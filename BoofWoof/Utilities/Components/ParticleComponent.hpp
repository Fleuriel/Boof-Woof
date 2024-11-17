#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Graphics/Mesh.h"
#include "../Core/Graphics/Shader.h"

#define PARTICLE_NUM 2

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
		Particle(glm::vec3 pos)
			: position(pos)
		{}
		~Particle() {};

		void update(float dt)
		{
			position += velocity * dt;
			lifeCount += dt;
		}
		
		glm::vec3 position{};
		glm::vec3 velocity{};
		glm::vec3 direction{};
		float lifeTime{};
		float lifeCount{};
	};

	void setMesh(Mesh mesh)
	{
		particle_mesh = mesh;
	}

	void init( )
	{
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			translation[i] = glm::vec3(0.0f, 0.0f, 0.0f);
			Particle ptc(glm::vec3(0.0f, 0.0f, 0.0f));
			ptc.velocity = glm::vec3(0.1f, 0.0f, 0.0f);
			ptc.direction = glm::vec3(0.0f, 0.0f, 0.0f);
			ptc.lifeTime = 0.0f;
			ptc.lifeCount = 0.0f;
			particles.emplace_back(ptc);
		}

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glGenVertexArrays(1, &quadVAO);
		//glGenBuffers(1, &quadVBO);
		//glGenBuffers(1, &quadEBO);

		glBindVertexArray(quadVAO);

		/*glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_mesh.vertices), &particle_mesh.vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(particle_mesh.indices), &particle_mesh.indices, GL_STATIC_DRAW);

		
		// vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		std::cout << "ParticleComponent: mesh loaded with " << particle_mesh.vertices.size() << " vertices\n";

		// instance VBO
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(2, 1);
		glBindVertexArray(0);*/

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glVertexAttribDivisor(0, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	}

	void update(float dt)
	{
		std::cout << "time: " << dt << "\n";
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			particles[i].update(dt);
			translation[i] = particles[i].position;
			std::cout << "translation: " << i << " : " << translation[i].x << " " << translation[i].y << " " << translation[i].z << "\n";
		}
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * PARTICLE_NUM, &translation[0] , GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		std::cout << "particle updated\n";
	}

	void draw(OpenGLShader shader)
	{
		shader.Use();
		glBindVertexArray(quadVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		//std::cout << "before particle draw\n";
		//glDrawArraysInstanced(GL_TRIANGLES, 0, particle_mesh.vertices.size(), PARTICLE_NUM);
		glDrawArraysInstanced(GL_POINTS, 0, 1, PARTICLE_NUM);
		//std::cout << "after particle draw\n";
		glBindVertexArray(0);
		shader.UnUse();
	}
	


	REFLECT_COMPONENT(ParticleComponent)
	{

	}


private:
	std::vector<Particle> particles{};
	glm::vec3 translation[PARTICLE_NUM]{};
	GLuint instanceVBO{}, quadVAO{}, quadVBO{}, quadEBO{};
	Mesh particle_mesh{};

	
};



#endif