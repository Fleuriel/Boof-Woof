#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Graphics/Mesh.h"
#include "../Core/Graphics/Shader.h"
#include <random>
#define PARTICLE_NUM 100



class ParticleComponent
{
public:
	ParticleComponent() {};
	~ParticleComponent() {
		particles.clear();
	};


	////// setter
	void setInitFlag(bool f) { init_flag = f; }
	void setDensity(float d) { density = d; }
	void setPosMin(glm::vec3 p) { Pos_min = p; }
	void setPosMax(glm::vec3 p) { Pos_max = p; }
	void setVelocityMin(float v) { velocity_min = v; }
	void setVelocityMax(float v) { velocity_max = v; }
	void setTargetPositions(std::vector<glm::vec3> tp) { target_positions = tp; }



	////// getter
	bool getInitFlag() { return init_flag; }
	float getDensity() { return density; }
	glm::vec3 getPosMin() { return Pos_min; }
	glm::vec3 getPosMax() { return Pos_max; }
	float getVelocityMin() { return velocity_min; }
	float getVelocityMax() { return velocity_max; }
	std::vector<glm::vec3> getTargetPositions() { return target_positions; }


	class Particle
	{
	public:
		Particle(glm::vec3 pos)
			: position(pos)
		{}
		~Particle() {};

		void update(float dt)
		{
			position += velocity * direction * dt;

		
			
			target_distance_count += velocity * dt;
			
			
			lifeCount += dt;
		}
		
		glm::vec3 position{};
		float velocity{};


		int target_count{};
		glm::vec3 direction{};
		float target_distance{};
		float target_distance_count{};

		float lifeTime{};
		float lifeCount{};
	};

	

	void init( )
	{
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			translation[i] = glm::vec3(0.0f, 0.0f, 0.0f);
			visibility[i] = 0.0f;
			Particle ptc(glm::vec3(0.0f, 0.0f, 0.0f));
			ptc.velocity = 0.f;
			ptc.target_count = 0;
			ptc.direction = glm::vec3(0.0f, 0.0f, 0.0f);
			ptc.lifeTime = 0.0f;
			ptc.lifeCount = 0.0f;
			particles.emplace_back(ptc);
		}

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);

		glGenBuffers(1, &visibilityVBO);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(float), &visibility[0], GL_STATIC_DRAW);


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

		// set life time 
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifeTime));
		glVertexAttribDivisor(0, 1);

		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glBindVertexArray(0);

	}

	void update(float dt)
	{
		density_counter += dt;
		bool add_particle = false;
		if (density_counter > density)
		{
			density_counter = 0.0f;
			add_particle = true;
			std::cout << "adding particle\n";
		}
		
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			if (add_particle && !visibility[i])
			{
				particles[i].position = getRandomVec3(Pos_min, Pos_max);
				particles[i].velocity = randomFloat(velocity_min, velocity_max);

				particles[i].target_count = 0;
				particles[i].direction = DirectionValue(particles[i].target_count);
				particles[i].target_distance = DistanceValue(particles[i].target_count);
				particles[i].target_distance_count = 0.0f;


				particles[i].lifeTime = 0.0f;
				particles[i].lifeCount = 0.0f;
				add_particle = false;
				visibility[i] = 1.0f;
			}
			particles[i].update(dt);
			if (particles[i].target_distance_count > particles[i].target_distance)
			{
				if (particles[i].target_count < target_positions.size()-1 )
				{
					particles[i].target_count++;
				}
				else {
					particles[i].target_count = target_positions.size() - 1;
					visibility[i] = 0.0f;

				}
				particles[i].target_distance_count = 0.0f;
				particles[i].direction = DirectionValue(particles[i].target_count);
				particles[i].target_distance = DistanceValue(particles[i].target_count);
			}
			translation[i] = particles[i].position;
			
		}
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * PARTICLE_NUM, &translation[0] , GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * PARTICLE_NUM, &visibility[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	}

	void draw()
	{
		
		glBindVertexArray(quadVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		//std::cout << "before particle draw\n";
		//glDrawArraysInstanced(GL_TRIANGLES, 0, particle_mesh.vertices.size(), PARTICLE_NUM);
		glDrawArraysInstanced(GL_POINTS, 0, 1, PARTICLE_NUM);
		//std::cout << "after particle draw\n";
		glBindVertexArray(0);
		
	}
	


	REFLECT_COMPONENT(ParticleComponent)
	{

	}

	// Utility function to generate a random float in range [min, max]
	float randomFloat(float min, float max) {
		static std::random_device rd;  // Obtain a random number from hardware
		static std::mt19937 eng(rd()); // Seed the generator
		if (min > max) {
			float temp = min;
			min = max;
			max = temp;
		}
		std::uniform_real_distribution<> distr(min, max); // Define the range
		return distr(eng);
	}

	// Function to generate a random glm::vec3 between two glm::vec3 vectors
	glm::vec3 getRandomVec3(glm::vec3 minVec, glm::vec3 maxVec) {
		glm::vec3 randomVec;
		randomVec.x = randomFloat(minVec.x, maxVec.x);
		randomVec.y = randomFloat(minVec.y, maxVec.y);
		randomVec.z = randomFloat(minVec.z, maxVec.z);
		return randomVec;
	}

	glm::vec3 DirectionValue(int target_c) {
		if (target_c < target_positions.size()-1)
		{
			glm::vec3 direction = target_positions[target_c + 1] - target_positions[target_c];
			return glm::normalize(direction);
		}
		else
		{
			glm::vec3 direction = target_positions[target_c] - target_positions[target_c - 1];
			return glm::normalize(direction);
		}

	}

	float DistanceValue(int target_c)
	{
		if (target_c < target_positions.size() - 1)
		{
			glm::vec3 direction = target_positions[target_c + 1] - target_positions[target_c];
			return glm::length(direction);
		}
		else
		{
			glm::vec3 direction = target_positions[target_c] - target_positions[target_c - 1];
			return glm::length(direction);
		}
		
	}

private:
	// particle data
	std::vector<Particle> particles{};
	glm::vec3 translation[PARTICLE_NUM]{};
	float visibility[PARTICLE_NUM]{};
	GLuint instanceVBO{}, visibilityVBO{}, quadVAO{}, quadVBO{}, quadEBO{};
	//Mesh particle_mesh{};

	// particle killer and generator
	float density_counter{};
	float lifeTime{};


	// settings
	bool init_flag = false;
	float density = 0.1f;

	glm::vec3 Pos_min{ 0.0f,-0.5f,0.0 };
	glm::vec3 Pos_max{ 0.0f, 0.5f,0.0 };
	float velocity_min{ 1.0f };
	float velocity_max{ 3.0f };

	std::vector<glm::vec3> target_positions{ {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} };
	
};



#endif