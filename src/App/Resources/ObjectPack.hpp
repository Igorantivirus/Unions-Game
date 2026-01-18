#pragma once

#include <filesystem>
#include <unordered_set>

#include <pugixml/pugixml.hpp>

#include "Core/FileWorker.hpp"
#include "TextureManager.hpp"
#include "Types.hpp"

#include <SDL3/SDL_log.h>

namespace resources
{
class ObjectPack
{
public:
    bool loadFromFolder(const std::filesystem::path &folderAbs, TextureManager &textures, const std::string &packName)
    {
        unload(textures);

        packName_ = packName;
        folderAbs_ = folderAbs;

        const auto configPath = folderAbs_ / "config.xml";

        std::string file;
        pugi::xml_document doc;
        if(!readXml(doc, file, configPath.string()))
        {
            unload(textures);
            return false;
        }

        const pugi::xml_node root = doc.child("root");
        if (!root)
        {
            unload(textures);
            return false;
        }

        std::unordered_set<std::string> loadedTextureKeys;

        for (const pugi::xml_node objectNode : root.children("object"))
        {
            ObjectDef def;

            def.id = objectNode.attribute("id").as_uint();
            def.level = objectNode.attribute("level").as_uint();
            def.points = objectNode.attribute("points").as_int(static_cast<int>(def.level));

            const std::string textureFile = objectNode.attribute("texture").as_string();
            if (def.id == 0 || textureFile.empty())
                continue;

            def.texturePath = packName_ + "/" + textureFile;

            const pugi::xml_node formNode = objectNode.child("form");
            if (!parseForm(formNode, def.form))
                continue;

            const auto texPath = folderAbs_ / textureFile;
            if (!textures.has(def.texturePath) && loadedTextureKeys.insert(def.texturePath).second)
            {
                if (!textures.load(def.texturePath, texPath))
                {
                    unload(textures);
                    return false;
                }
            }
            textureKeys_.insert(def.texturePath);

            objects_[def.id] = std::move(def);
        }

        if (objects_.empty())
        {
            unload(textures);
            return false;
        }
        return true;
    }

    void unload(TextureManager &textures)
    {
        for (const auto &key : textureKeys_)
            textures.unload(key);
        textureKeys_.clear();
        objects_.clear();
        packName_.clear();
        folderAbs_.clear();
    }

    const ObjectDef *getById(const IDType id) const
    {
        auto it = objects_.find(id);
        return it == objects_.end() ? nullptr : &it->second;
    }

    const std::unordered_map<IDType, ObjectDef> &getAll() const
    {
        return objects_;
    }

    const std::filesystem::path &getFolder() const
    {
        return folderAbs_;
    }

    const std::string &getName() const
    {
        return packName_;
    }

private:
    static bool parseForm(const pugi::xml_node &formNode, ObjectFormDef &out)
    {
        if (!formNode)
            return false;

        const std::string type = formNode.attribute("type").as_string();
        if (type == "circle" || type == "circel")
        {
            out.type = ObjectFormType::Circle;
            float r = formNode.attribute("radius").as_float(0.f);
            out.form = r;
            return r > 0.0f;
        }
        if (type == "ellipse")
        {
            out.type = ObjectFormType::Ellipse;
            const float rx = formNode.attribute("rx").as_float(0.f);
            const float ry = formNode.attribute("ry").as_float(0.f);
            out.form = sdl3::Vector2f{rx, ry};
            return rx > 0.0f && ry > 0.0f;
        }
        if (type == "poligon8" || type == "polygon8")
        {
            out.type = ObjectFormType::Polygon8;
            std::vector<sdl3::Vector2f> vertices;
            vertices.reserve(8);

            for (const pugi::xml_node v : formNode.children())
            {
                const std::string name = v.name();
                if (name != "vertex")
                    continue;

                const float x = v.attribute("x").as_float();
                const float y = v.attribute("y").as_float();
                vertices.push_back({x, y});
                if (vertices.size() == 8)
                    break;
            }
            out.form = vertices;
            return vertices.size() >= 3;
        }

        return false;
    }

    static bool readXml(pugi::xml_document &doc, std::string &file, const std::string_view configPath)
    {
        sdl3io::FileWorker read(configPath, sdl3io::FileWorkerMode::read | sdl3io::FileWorkerMode::binary);
        if (!read.isOpen())
        {
            SDL_Log("Couldn't open the file");
            return false;
        }
        file = read.readAll();
        const pugi::xml_parse_result res = doc.load_string(file.c_str());
        if (!res)
        {
            return false;
        }
        return true;
    }

private:
    std::string packName_;
    std::filesystem::path folderAbs_;
    std::unordered_map<IDType, ObjectDef> objects_;
    std::unordered_set<std::string> textureKeys_;
};
} // namespace resources
