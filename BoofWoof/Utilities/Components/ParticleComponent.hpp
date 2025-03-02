#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Graphics/Mesh.h"
#include "../Core/Graphics/Shader.h"
#include "../Core/ResourceManager/ResourceManager.h"
#include <random>
#define PARTICLE_NUM 100



//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



enum class ParticleType
{
	TEXTURED_3D,
	TEXTURED,
	POINT,
	UNKNOWN,
};

class ParticleComponent
{
public:
	ParticleComponent() {};
	ParticleComponent(float d, glm::vec3 p_min, glm::vec3 p_max,
		float v_min, float v_max, std::vector<glm::vec3> target_positions,
		float p_size, glm::vec3 p_color, float oct, ParticleType pt, std::string modelname, std::string texturename)
		: density(d), Pos_min(p_min), Pos_max(p_max),
		velocity_min(v_min), velocity_max(v_max), target_positions(target_positions),
		particle_size(p_size), particle_color(p_color), opacity(oct), particle_type(pt), particle_modelname(modelname), particle_texturename(texturename) {};
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
	void setParticleSize(float s) { particle_size = s; }
	void setParticleColor(glm::vec3 c) { particle_color = c; }
	void setOpacity(float o) { opacity = o; }
	
	void setParticleType(ParticleType pt) { 
		particle_type = pt;
		free();
		init_flag = false;
	}
	void setTypeString(std::string type) {
		if (type == "TEXTURED_3D") {
			setParticleType(ParticleType::TEXTURED_3D);
		}
		else if (type == "TEXTURED") {
			setParticleType(ParticleType::TEXTURED);
		}
		else if (type == "POINT") {
			setParticleType(ParticleType::POINT);
		}
	}
	void setParticleModelname(std::string modelname) { 
		particle_modelname = modelname; 
		free();
		init_flag = false;
	}
	void setParticleTexturename(std::string texturename) { particle_texturename = texturename; }


	


	////// getter
	bool getInitFlag() { return init_flag; }
	float getDensity() { return density; }
	glm::vec3 getPosMin() { return Pos_min; }
	glm::vec3 getPosMax() { return Pos_max; }
	float getVelocityMin() { return velocity_min; }
	float getVelocityMax() { return velocity_max; }
	std::vector<glm::vec3> getTargetPositions() { return target_positions; }
	float getParticleSize() { return particle_size; }
	glm::vec3 getParticleColor() { return particle_color; }
	float getOpacity() { return opacity; }
	ParticleType getParticleType() {	return particle_type;}
	std::string getTypeString() {
		switch (particle_type)
		{
		case ParticleType::TEXTURED_3D:
			return "TEXTURED_3D";
			break;
		case ParticleType::TEXTURED:
			return "TEXTURED";
			break;
		case ParticleType::POINT:
			return "POINT";
			break;
		default:
			return "UNKNOWN";  // Handle unknown cases
			break;
		}
	}
	std::string getParticleModelname() { return particle_modelname; }
	std::string getParticleTexturename() { return particle_texturename; }

	static ParticleType stringToParticleType(std::string type) {
		if (type == "TEXTURED_3D") {
			return ParticleType::TEXTURED_3D;
		}
		else if (type == "TEXTURED") {
			return ParticleType::TEXTURED;
		}
		else if (type == "POINT") {
			return ParticleType::POINT;
		}
		// Add a default return value (assuming UNKNOWN exists)
		return ParticleType::UNKNOWN;  // You may need to define this in your ParticleType enum
	}


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

	void init(const Mesh * input_mesh = nullptr) {
		switch (particle_type)
		{
		case ParticleType::TEXTURED_3D:
			init_3Dobj(*input_mesh);
			break;
		case ParticleType::TEXTURED:
			init_textured();
			break;
		case ParticleType::POINT:
			init_point();
			break;
		}
	}


	void init_point() {
		// init individual particles
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


		// instance VBO
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// visibility VBO
		glGenBuffers(1, &visibilityVBO);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(float), &visibility[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);


		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glVertexAttribDivisor(0, 1);


		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glBindVertexArray(0);
	}

	void init_textured( )
	{
		// init individual particles
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


		// instance VBO
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// visibility VBO
		glGenBuffers(1, &visibilityVBO);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(float), &visibility[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// set mesh
		float quadVertices[] = {
			// positions			        // texture coords
			-0.5f,  0.5f, 0.0f,   1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,    0.0f, 1.0f,

			-0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f,   0.0f, 0.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		
		// set instance VBO
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); 
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glVertexAttribDivisor(3, 1); 

		// set visibility VBO
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(4, 1);

		glBindVertexArray(0);


	}

	void init_3Dobj(const Mesh& mesh) {

		particle_mesh = mesh;

		// init individual particles
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

		// instance VBO
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(glm::vec3), &translation[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// visibility VBO
		glGenBuffers(1, &visibilityVBO);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUM * sizeof(float), &visibility[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadEBO);

		glBindVertexArray(quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, visibilityVBO);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
		glVertexAttribDivisor(4, 1);

		glBindVertexArray(0);

	}

	void update_textured(float dt)
	{
		density_counter += dt;
		bool add_particle = false;
		if (density_counter > density)
		{
			density_counter = 0.0f;
			add_particle = true;
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
					particles[i].target_count = static_cast<int>(target_positions.size() - 1);
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

	void draw() {
		switch (particle_type)
		{
		case ParticleType::TEXTURED_3D:
			draw_3Dobj();
			break;
		case ParticleType::TEXTURED:
			draw_textured();
			break;
		case ParticleType::POINT:
			draw_point();
			break;
		}
	}

	void draw_textured()
	{
		
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, PARTICLE_NUM);
		glBindVertexArray(0);
		
	}

	void draw_3Dobj() {
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(particle_mesh.vertices.size()), PARTICLE_NUM);
		glBindVertexArray(0);
	}

	void draw_point() {
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_POINTS, 0, 1, PARTICLE_NUM);
		glBindVertexArray(0);
	}

	void free() {
		glDeleteBuffers(1, &instanceVBO);
		glDeleteBuffers(1, &visibilityVBO);
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &quadEBO);
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
		return static_cast<float>(distr(eng));
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
	float visibility[PARTICLE_NUM]{ 1.0f };
	GLuint instanceVBO{}, visibilityVBO{}, quadVAO{}, quadVBO{}, quadEBO{};
	
	
	Mesh particle_mesh{};
	std::string particle_modelname = "Bed";
	
	std::string particle_texturename = "Bed";


	ParticleType particle_type{ ParticleType::POINT };


	// particle killer and generator
	float density_counter{};


	// settings
	bool init_flag = false;

	float particle_size = 10.f;
	glm::vec3 particle_color{ 1.0f, 1.0f, 1.0f };
	float opacity = 1.0f;

	float density = 0.1f;

	glm::vec3 Pos_min{ 0.0f,-0.5f,0.0 };
	glm::vec3 Pos_max{ 0.0f, 0.5f,0.0 };
	float velocity_min{ 1.0f };
	float velocity_max{ 3.0f };

	std::vector<glm::vec3> target_positions{ {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} };
	
};



#endif