#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#include "../BoofWoof/Core/ECS/Coordinator.hpp"
#include "../BoofWoof/Core/Windows/WindowManager.h"
#include "../BoofWoof/Utilities/Components/TransformComponent.hpp"
#include "../BoofWoof/Utilities/Components/MetaData.hpp"
#include "../BoofWoof/Core/SceneManager/SceneManager.h"
#include "EngineCore.h"
#include "../BoofWoof/Utilities/Components/AudioComponent.hpp"
#include "../BoofWoof/Utilities/Components/BehaviourComponent.hpp"
#include "UndoRedoManager.hpp"
#include "ArchetypeManager.hpp"

#include <filesystem>

struct TextureDescriptor {
	std::string textureAlpha;
	bool alphaIsTransparency;
	std::string nonPowerOfTwoOption;
	bool readWrite;
	bool generateMipMaps;
	bool mipMapLimits;
	std::string mipMapLimitGroup;
	bool mipStreaming;
	std::string mipMapFiltering;
	bool preserveCoverage;
	bool replicateBorder;
	bool fadeoutToGray;
	bool ignorePngGamma;
	std::string wrapMode;
	std::string filterMode;
	float anisoLevel;
	std::string maxSize;
	std::string resizeAlgorithm;
	std::string format;
	std::string compressionQuality;
	bool useCrunchCompression;

	bool SaveTextureDescriptor(const std::string& filepath, const TextureDescriptor& descriptor) {
		using namespace rapidjson;

		Document document;
		document.SetObject();
		Document::AllocatorType& allocator = document.GetAllocator();

		document.AddMember("textureAlpha", descriptor.textureAlpha, allocator);
		document.AddMember("alphaIsTransparency", descriptor.alphaIsTransparency, allocator);
		document.AddMember("nonPowerOfTwoOption", descriptor.nonPowerOfTwoOption, allocator);
		document.AddMember("readWrite", descriptor.readWrite, allocator);
		document.AddMember("generateMipMaps", descriptor.generateMipMaps, allocator);
		document.AddMember("mipMapLimits", descriptor.mipMapLimits, allocator);
		document.AddMember("mipMapLimitGroup", descriptor.mipMapLimitGroup, allocator);
		document.AddMember("mipStreaming", descriptor.mipStreaming, allocator);
		document.AddMember("mipMapFiltering", descriptor.mipMapFiltering, allocator);
		document.AddMember("preserveCoverage", descriptor.preserveCoverage, allocator);
		document.AddMember("replicateBorder", descriptor.replicateBorder, allocator);
		document.AddMember("fadeoutToGray", descriptor.fadeoutToGray, allocator);
		document.AddMember("ignorePngGamma", descriptor.ignorePngGamma, allocator);
		document.AddMember("wrapMode", descriptor.wrapMode, allocator);
		document.AddMember("filterMode", descriptor.filterMode, allocator);
		document.AddMember("anisoLevel", descriptor.anisoLevel, allocator);
		document.AddMember("maxSize", descriptor.maxSize, allocator);
		document.AddMember("resizeAlgorithm", descriptor.resizeAlgorithm, allocator);
		document.AddMember("format", descriptor.format, allocator);
		document.AddMember("compressionQuality", descriptor.compressionQuality, allocator);
		document.AddMember("useCrunchCompression", descriptor.useCrunchCompression, allocator);

		// Write to file
		FILE* fp = fopen(filepath.c_str(), "w");
		if (!fp) {
			std::cerr << "Failed to open descriptor file for writing: " << filepath << std::endl;
			return false;
		}

		char writeBuffer[65536];
		FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		PrettyWriter<FileWriteStream> writer(os);
		document.Accept(writer);

		fclose(fp);
		return true;
	}

	bool LoadTextureDescriptor(const std::string& filepath, TextureDescriptor& descriptor) {
		using namespace rapidjson;

		// Open the file
		FILE* fp = fopen(filepath.c_str(), "r");
		if (!fp) {
			std::cerr << "Failed to open descriptor file: " << filepath << std::endl;
			return false;
		}

		// Set up read buffer and parse the JSON file
		char readBuffer[65536];
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		Document document;
		document.ParseStream(is);
		fclose(fp);

		// Check for parsing errors
		if (document.HasParseError()) {
			std::cerr << "Error parsing descriptor file: " << filepath << std::endl;
			return false;
		}

		// Read each field with the appropriate type conversions
		if (document.HasMember("textureAlpha") && document["textureAlpha"].IsString())
			descriptor.textureAlpha = document["textureAlpha"].GetString();

		if (document.HasMember("alphaIsTransparency") && document["alphaIsTransparency"].IsBool())
			descriptor.alphaIsTransparency = document["alphaIsTransparency"].GetBool();

		if (document.HasMember("nonPowerOfTwoOption") && document["nonPowerOfTwoOption"].IsString())
			descriptor.nonPowerOfTwoOption = document["nonPowerOfTwoOption"].GetString();

		if (document.HasMember("readWrite") && document["readWrite"].IsBool())
			descriptor.readWrite = document["readWrite"].GetBool();

		if (document.HasMember("generateMipMaps") && document["generateMipMaps"].IsBool())
			descriptor.generateMipMaps = document["generateMipMaps"].GetBool();

		if (document.HasMember("mipMapLimits") && document["mipMapLimits"].IsBool())
			descriptor.mipMapLimits = document["mipMapLimits"].GetBool();

		if (document.HasMember("mipMapLimitGroup") && document["mipMapLimitGroup"].IsString())
			descriptor.mipMapLimitGroup = document["mipMapLimitGroup"].GetString();

		if (document.HasMember("mipStreaming") && document["mipStreaming"].IsBool())
			descriptor.mipStreaming = document["mipStreaming"].GetBool();

		if (document.HasMember("mipMapFiltering") && document["mipMapFiltering"].IsString())
			descriptor.mipMapFiltering = document["mipMapFiltering"].GetString();

		if (document.HasMember("preserveCoverage") && document["preserveCoverage"].IsBool())
			descriptor.preserveCoverage = document["preserveCoverage"].GetBool();

		if (document.HasMember("replicateBorder") && document["replicateBorder"].IsBool())
			descriptor.replicateBorder = document["replicateBorder"].GetBool();

		if (document.HasMember("fadeoutToGray") && document["fadeoutToGray"].IsBool())
			descriptor.fadeoutToGray = document["fadeoutToGray"].GetBool();

		if (document.HasMember("ignorePngGamma") && document["ignorePngGamma"].IsBool())
			descriptor.ignorePngGamma = document["ignorePngGamma"].GetBool();

		if (document.HasMember("wrapMode") && document["wrapMode"].IsString())
			descriptor.wrapMode = document["wrapMode"].GetString();

		if (document.HasMember("filterMode") && document["filterMode"].IsString())
			descriptor.filterMode = document["filterMode"].GetString();

		if (document.HasMember("anisoLevel") && document["anisoLevel"].IsFloat())
			descriptor.anisoLevel = document["anisoLevel"].GetFloat();

		if (document.HasMember("maxSize") && document["maxSize"].IsString())
			descriptor.maxSize = document["maxSize"].GetString();

		if (document.HasMember("resizeAlgorithm") && document["resizeAlgorithm"].IsString())
			descriptor.resizeAlgorithm = document["resizeAlgorithm"].GetString();

		if (document.HasMember("format") && document["format"].IsString())
			descriptor.format = document["format"].GetString();

		if (document.HasMember("compressionQuality") && document["compressionQuality"].IsString())
			descriptor.compressionQuality = document["compressionQuality"].GetString();

		if (document.HasMember("useCrunchCompression") && document["useCrunchCompression"].IsBool())
			descriptor.useCrunchCompression = document["useCrunchCompression"].GetBool();

		return true;
	}
};


class ImGuiEditor 
{
public:
	ImGuiEditor() = default;
	~ImGuiEditor() = default;

	void ImGuiInit(Window* window);
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiEnd();

	// Panels
	void ImGuiViewport();
	void WorldHierarchy();
	void InspectorWindow();
	void AssetWindow();
	void Settings();
	void Scenes();
	void Audio();
	void PlayStopRunBtn();
	void ArchetypeTest();

	// PlayStopRun Panel
	enum class States
	{
		Play,
		Stop,
		Pause
	};
	States m_State = States::Stop;

	// Audio Panel
	std::string m_AudioName{};
	bool m_ShowAudio = false;

	// For fun
	void PlotSystemDT(const char* name, float dt, float totalDT);

	// Variables
	Entity g_SelectedEntity = 0;
	Entity g_GettingDeletedEntity = static_cast<Entity>(-1);		// will be deleted from the back
		
	std::filesystem::path m_BaseDir = "../BoofWoof/Assets";			// Asset Panel
	std::filesystem::path m_CurrDir = m_BaseDir;
	std::string m_LastOpenedFile{};

	// For saving files
	std::string m_FileName{};
	std::string m_FinalFileName{};
	std::string m_FilePath{};

	bool m_IsSelected{ false };
	bool m_PlayerExist{ false };

	static ImGuiEditor& GetInstance();

private:
	Window* m_Window = nullptr;
	std::vector<std::type_index> compTypes; // Member variable to hold selected component types
	std::filesystem::path m_SelectedFile;



};

#endif  // IMGUIEDITOR_H