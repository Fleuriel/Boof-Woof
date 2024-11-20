/**************************************************************************
 * @file Descriptor.h
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file declares the Descriptor classes
 *
 *************************************************************************/

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H


struct TextureDescriptor {

    int textureType = 0;
    int textureShape = 0;
    bool sRGB = false;
    int alphaSource = 0;
    bool alphaIsTransparency = false;
    int nonPowerOfTwo = 0;
    bool readWrite = true;
    bool generateMipMaps = true;
    bool mipMapLimits = false;
    int mipMapLimitGroup = 0;
    bool mipStreaming = false;
    int mipMapFiltering = 0;
    bool preserveCoverage = true;
    bool replicateBorder = false;
    bool fadeoutToGray = false;
    bool ignorePngGamma = false;
    int wrapMode = 0;
    int filterMode = 0;
    float anisoLevel = 1.0f;
    int maxSize = 0;
    int resizeAlgorithm = 0;
    int format = 10;
    int compression = 0;
    bool useCrunchCompression = false;

    // Save the current descriptor to a file
    bool SaveTextureDescriptor(const std::string& filepath);

    // Load descriptor data from a file into this instance
    bool LoadTextureDescriptor(const std::string& filepath);
};

struct MaterialDescriptor
{

    
    int shaderIndex;            // Select Shader
    std::string shaderChosen;
    
    float albedoColorRed;       // 0 ~ 1
    float albedoColorGreen;     // 0 ~ 1
    float albedoColorBlue;      // 0 ~ 1
    float albedoColorAlpha;     // 0 ~ 1

    float metallic;             // 0 ~ 1
    float smoothness;           // 0 ~ 1

    int materialAlpha;          // Select Albedo Stuffs.


    MaterialDescriptor() : shaderIndex(-1), albedoColorRed(0), albedoColorGreen(0), albedoColorBlue(0), albedoColorAlpha(0),
        metallic(0), smoothness(0), materialAlpha(0), shaderChosen("nothing"){}


    // Save the current descriptor to a file
    bool SaveMaterialDescriptor(const std::string& filepath);

    // Load descriptor data from a file into this instance
    bool LoadMaterialDescriptor(const std::string& filepath);



};



#endif // !DESCRIPTOR_H