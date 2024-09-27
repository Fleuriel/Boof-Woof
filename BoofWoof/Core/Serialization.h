#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

class Serialization {
public:
    static void SaveEngineState(const std::string& filepath);
    static void LoadEngineState(const std::string& filepath);
};

#endif // SERIALIZATION_H
