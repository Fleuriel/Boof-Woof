/**************************************************************************
 * @file Sprite.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file defines the sprite class
 *
 *************************************************************************/

#include "Sprite.h"

/**************************************************************************
 * @brief Constructor for the Sprite class.
 *
 * This constructor initializes a new instance of the `Sprite` class. It sets
 * default values for various properties of the sprite, including the
 * texture, number of rows, columns, frame width, frame height, texture
 * width, texture height, and the container for storing bottom-left
 * coordinates.
 *
 * @param None.
 *
 * @return None.
 *************************************************************************/
Sprite::Sprite() {

}

/**************************************************************************
 * @brief Destructor for the Sprite class.
 *
 * This destructor is responsible for cleaning up resources associated with
 * an instance of the `Sprite` class. It is called when a `Sprite` object is
 * no longer needed, and it ensures the proper release of any resources
 * held by the sprite.
 *
 * @param None.
 *
 * @return None.
 *************************************************************************/
Sprite::~Sprite() {

}

/**************************************************************************
 * @brief Sets the texture to be used by the sprite.
 *
 * This method sets the texture to be used by the sprite to the provided
 * texture handler. It updates the internal texture property with the given
 * texture handler and returns true if the texture is set (non-empty).
 * Otherwise, it returns false.
 *
 * @param int tex - The texture handler to be set for the sprite.
 *
 * @return bool - true if the texture is set (non-empty), false otherwise.
 *************************************************************************/
bool Sprite::SetTexture(int tex) {
    // Set the texture
    texture = tex;
    // Returns true if texture is not empty
    return !texture;
}

/**************************************************************************
 * @brief Retrieves the texture handler used by the sprite.
 *
 * This method returns the texture handler currently used by the sprite.
 *
 * @param None.
 *
 * @return int - The texture handler used by the sprite.
 *************************************************************************/
int Sprite::GetTexture() {
    // Returns the texture
    return texture;
}

/**************************************************************************
 * @brief Sets the number of rows and columns for the sprite.
 *
 * This method sets the number of rows and columns for the sprite's animation frames.
 *
 * @param int a - The number of rows.
 * @param int b - The number of columns.
 *
 * @return bool - True if both rows and columns are set and non-zero, false otherwise.
 *************************************************************************/
bool Sprite::SetRowsAndColumns(int a, int b) {
    // Set the rows
    rows = a;
    // Set the columns
    columns = b;
    // Returns true if both rows and columns are non-zero
    return (rows && columns);
}

/**************************************************************************
 * @brief Retrieves the number of rows for the sprite's animation frames.
 *
 * This method returns the number of rows used in the sprite's animation frames.
 *
 * @return int - The number of rows.
 *************************************************************************/
int Sprite::GetRows() {
    return rows;
}

/**************************************************************************
 * @brief Retrieves the number of columns for the sprite's animation frames.
 *
 * This method returns the number of columns used in the sprite's animation frames.
 *
 * @return int - The number of columns.
 *************************************************************************/
int Sprite::GetColumns() {
    return columns;
}

/**************************************************************************
 * @brief Sets the frame width and height for the sprite.
 *
 * This method sets the width and height for the individual animation frames of the sprite.
 *
 * @param int a - The frame width.
 * @param int b - The frame height.
 * @return bool - true if both width and height are non-zero, false otherwise.
 *************************************************************************/
bool Sprite::SetWidthAndHeight(int a, int b) {
    // Set the width
    width = a;
    // Set the height
    height = b;
    // Returns true if both width and height are non-zero
    return (width && height);
}

/**************************************************************************
 * @brief Retrieves the frame width of the sprite.
 *
 * This method retrieves the width of the individual animation frames of the sprite.
 *
 * @return int - The frame width.
 *************************************************************************/
int Sprite::GetWidth() {
    return width;
}

/**************************************************************************
 * @brief Retrieves the frame height of the sprite.
 *
 * This method retrieves the height of the individual animation frames of the sprite.
 *
 * @return int - The frame height.
 *************************************************************************/
int Sprite::GetHeight() {
    return height;
}

/**************************************************************************
 * @brief Sets the texture width and height for the sprite.
 *
 * This method sets the width and height of the texture that the sprite uses.
 *
 * @param a - The width of the texture.
 * @param b - The height of the texture.
 * @return bool - True if both width and height are non-zero, false otherwise.
 *************************************************************************/
bool Sprite::SetTextureWidthAndHeight(int a, int b) {
    // Set the width
    texWidth = a;
    // Set the height
    texHeight = b;
    // Returns true if both width and height are non-zero
    return (texWidth && texHeight);
}

/**************************************************************************
 * @brief Retrieves the width of the texture used by the sprite.
 *
 * This method retrieves the width of the texture used by the sprite.
 *
 * @return int - The width of the texture.
 *************************************************************************/
int Sprite::GetTextureWidth() {
    return texWidth;
}

/**************************************************************************
 * @brief Retrieves the height of the texture used by the sprite.
 *
 * This method retrieves the height of the texture used by the sprite.
 *
 * @return int - The height of the texture.
 *************************************************************************/
int Sprite::GetTextureHeight() {
    return texHeight;
}

/**************************************************************************
 * @brief Stores bottom-left coordinates in the sprite's coordinate container.
 *
 * This method stores the specified bottom-left coordinates in the sprite's
 * coordinate container.
 *
 * @param a - The x-coordinate of the bottom-left position.
 * @param b - The y-coordinate of the bottom-left position.
 * @return bool - True if the coordinates were successfully added.
 *************************************************************************/
bool Sprite::AddCoordinates(int a, int b) {
    // Get the original size of the coordinates container
    size_t size = coordinates.size();
    // Store the new coordinates into the coordinates container
    coordinates.push_back(std::pair<int, int>(a, b));
    // Returns true if the coordinates container size increased (new coordinates sucessfully stored)
    return (coordinates.size() > size);
}
