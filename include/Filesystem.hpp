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
        std::string path = getCurrentWorkingDirectory().string();

        const size_t position = path.find_last_of('/');

        path = path.substr(0, position);

        path += "/resources";

        return {path};
    }

    inline std::filesystem::path getSkyboxesFolderPath()
    {
        return {getResourcesFolderPath().string() + "/skyboxes"};
    }

    inline std::filesystem::path getMaterialsFolderPath()
    {
        return {getResourcesFolderPath().string() + "/materials"};
    }

    inline std::filesystem::path getAnimationsFolderPath()
    {
        return {getResourcesFolderPath().string() + "/animations"};
    }

    inline std::filesystem::path getTexturesFolderPath()
    {
        return {getResourcesFolderPath().string() + "/textures"};
    }

    inline std::filesystem::path getModelsFolderPath()
    {
        return {getResourcesFolderPath().string() + "/models"};
    }

    inline std::filesystem::path getMapsFolderPath()
    {
        return {getResourcesFolderPath().string() + "/maps"};
    }

    inline std::filesystem::path getShadersFolderPath()
    {
        return {getResourcesFolderPath().string() + "/shaders"};
    }

    inline std::filesystem::path getFontsFolderPath()
    {
        return {getResourcesFolderPath().string() + "/fonts"};
    }
}

#endif //FILESYSTEM_HPP