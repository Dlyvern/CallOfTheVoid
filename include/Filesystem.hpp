#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <fstream>
#include <filesystem>

namespace filesystem
{
    inline std::string getFileContents(const std::string& path)
    {
        //TODO: MAKE IT MORE SAFE
        std::ifstream file(path);
        std::string str, line;

        while (std::getline(file, line)) 
            str += line + "\n";

        return str;
    }

    inline std::filesystem::path getCurrentWorkingDirectory()
    {
        return std::filesystem::current_path();
    }

    inline std::filesystem::path getResourcesFolderPath()
    {
        return std::filesystem::path("/home/dlyvern/Projects/CallOfTheVoid/resources");
    }

    inline std::filesystem::path getTexturesFolderPath()
    {
        return std::filesystem::path(getResourcesFolderPath().string() + "/textures");
    }

    inline std::filesystem::path getModelsFolderPath()
    {
        return std::filesystem::path(getResourcesFolderPath().string() + "/models");
    }

    inline std::filesystem::path getMapsFolderPath()
    {
        return std::filesystem::path(getResourcesFolderPath().string() + "/maps");
    }

    inline std::filesystem::path getShadersFolderPath()
    {
        return std::filesystem::path(getResourcesFolderPath().string() + "/shaders");
    }

    inline std::filesystem::path getFontsFolderPath()
    {
        return std::filesystem::path(getResourcesFolderPath().string() + "/fonts");
    }
}

#endif //FILESYSTEM_HPP