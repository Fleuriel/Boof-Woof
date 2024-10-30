#pragma once
#ifndef LIGHT_H
#define LIGHT_H

class Light
{
public:
	Light(glm::vec3 position, glm::vec3 color)
		: m_Position(position), m_Color(color) {/*Empty by design*/
	}

	~Light() = default;

	void SetPosition(glm::vec3 position) { this->m_Position = position; }
	void SetColor(glm::vec3 color) { this->m_Color = color; }

	glm::vec3& GetPosition() { return m_Position; }
	glm::vec3& GetColor() { return m_Color; }


private:
	glm::vec3 m_Position;
	glm::vec3 m_Color;


};

#endif