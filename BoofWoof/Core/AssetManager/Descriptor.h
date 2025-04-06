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
    std::string materialName;
    std::string materialNameMat;



    int shaderIndex;            // Select Shader
    std::string shaderChosen;

    float albedoColorRed;       // 0 ~ 1
    float albedoColorGreen;     // 0 ~ 1
    float albedoColorBlue;      // 0 ~ 1
    float albedoColorAlpha;     // 0 ~ 1
    float finalAlpha;


    float metallic;             // 0 ~ 1
    float smoothness;           // 0 ~ 1

    int materialAlpha;          // Select Albedo Stuffs.

    int DiffuseID;
    int NormalID;
    int HeightID;

    std::string textureDiffuse;
    std::string textureNormal;
    std::string textureHeight;

    float gammaValue;


    MaterialDescriptor() : shaderIndex(6), albedoColorRed(1.0), albedoColorGreen(1.0), albedoColorBlue(1.0), albedoColorAlpha(1.0), finalAlpha(1),
        metallic(0), smoothness(0), materialAlpha(0), shaderChosen("Direction_obj_render"), DiffuseID(-1), NormalID(-1), HeightID(-1), textureDiffuse("NothingTexture"),
        textureNormal("NothingNormal"), textureHeight("NothingHeight") , gammaValue(2.2f){ }


    // Save the current descriptor to a file
    bool SaveMaterialDescriptor(const std::string& filepath);

    // Load descriptor data from a file into this instance
    bool LoadMaterialDescriptor(const std::string& filepath);



};



#endif // !DESCRIPTOR_H