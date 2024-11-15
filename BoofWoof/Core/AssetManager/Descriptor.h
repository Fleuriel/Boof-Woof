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
    std::string textureAlpha = "default_alpha";              // Default texture alpha
    bool alphaIsTransparency = false;                        // Default transparency flag
    std::string nonPowerOfTwoOption = "default_option";      // Default option for non-power-of-two textures
    bool readWrite = true;                                   // Default read/write flag
    bool generateMipMaps = true;                             // Default mipmap generation flag
    bool mipMapLimits = false;                               // Default mipmap limits flag
    std::string mipMapLimitGroup = "default_group";          // Default mipmap limit group
    bool mipStreaming = false;                               // Default mip streaming flag
    std::string mipMapFiltering = "default_filtering";       // Default mipmap filtering
    bool preserveCoverage = true;                            // Default preserve coverage flag
    bool replicateBorder = false;                            // Default replicate border flag
    bool fadeoutToGray = false;                              // Default fadeout to gray flag
    bool ignorePngGamma = false;                             // Default ignore PNG gamma flag
    std::string wrapMode = "default_wrap";                   // Default wrap mode
    std::string filterMode = "default_filter";               // Default filter mode
    float anisoLevel = 1.0f;                                 // Default anisotropic level
    std::string maxSize = "1024";                            // Default max size for texture
    std::string resizeAlgorithm = "default_algorithm";       // Default resize algorithm
    std::string format = "default_format";                   // Default texture format
    std::string compressionQuality = "default_quality";      // Default compression quality
    bool useCrunchCompression = false;                       // Default use crunch compression flag

    // Save the current descriptor to a file
    bool SaveTextureDescriptor(const std::string& filepath);

    // Load descriptor data from a file into this instance
    bool LoadTextureDescriptor(const std::string& filepath);
};



#endif // !DESCRIPTOR_H