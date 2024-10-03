#include <vector>

class Sprite
{
public:

	Sprite();																						//Constructor for sprite class
	~Sprite();																						//Destructor for sprite class

	bool SetTexture(int);																			//Sets the texture to be used
	int GetTexture();																				//Retrieves the texture to be used

	bool SetRowsAndColumns(int, int);																//Sets the number of rows and columns
	int GetRows();																					//Retrieves the number of rows
	int GetColumns();																				//Retrieves the number of columns

	bool SetWidthAndHeight(int, int);																//Sets the frame width and height
	int GetWidth();																					//Retrieves the frame width
	int GetHeight();																				//Retrieves the frame height

	bool SetTextureWidthAndHeight(int, int);														//Sets the texture width and height
	int GetTextureWidth();																			//Retrieves the texture width
	int GetTextureHeight();																			//Retrieves the texture height

	bool AddCoordinates(int, int);																	//Stores coordinates into the coordinate container

private:
	int texture{};																					//Stores the texture to be used
	int rows{};																						//Stores number of rows
	int columns{};																					//Stores number of columns
	int frameNum{};																					//Stores frame index
	int width{};																					//Stores frame width
	int height{};																					//Stores frame height
	int texWidth{};																					//Stores texture width
	int texHeight{};																				//Stores texture height
	std::vector<std::pair<int, int>> coordinates{};													//Stores bottom left coordinates
};