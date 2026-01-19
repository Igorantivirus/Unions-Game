#pragma once

#include <SDL3/SDL_error.h>
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

        pugi::xml_document doc;
        if (!readXml(doc, configPath.string()))
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

            const pugi::xml_node meta = objectNode.child("meta");
            const pugi::xml_node filler = objectNode.child("filler");
            const pugi::xml_node form = objectNode.child("form");

            def.id = objectNode.attribute("id").as_uint();
            def.level = meta.attribute("level").as_uint();
            def.points = meta.attribute("points").as_int(static_cast<int>(def.level));

            if (!parseFiller(filler, def.filler) || !parseForm(form, def.form))
            {
                unload(textures);
                SDL_Log("Error of parseFiller or parseForm\ns");
                return false;
            }

            if (def.filler.type == ObjectFillerType::Texture)
            {
                const std::string fileName = def.filler.getTextureName();
                const std::string texturePathKey = packName_ + '/' + fileName;
                const auto texPath = folderAbs_ / fileName;
                def.filler.filler = texturePathKey;
                if (!textures.has(texturePathKey) && loadedTextureKeys.insert(texturePathKey).second)
                {
                    if (!textures.load(texturePathKey, texPath))
                    {
                        unload(textures);
                        return false;
                    }
                }
                textureKeys_.insert(texturePathKey);

                objects_[def.id] = std::move(def);
            }
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
    static bool parseFiller(const pugi::xml_node &filler, ObjectFillerDef &out)
    {
        if (!filler)
            return false;
        const std::string_view type = filler.attribute("type").as_string();
        if (type == "texture")
        {
            out.type = ObjectFillerType::Texture;
            std::string texture = filler.attribute("texture").as_string();
            out.filler = texture;
            return !texture.empty();
        }
        return false;
    }
    static bool parseForm(const pugi::xml_node &form, ObjectFormDef &out)
    {
        if (!form)
            return false;

        const std::string_view type = form.attribute("type").as_string();
        if (type == "circle")
        {
            out.type = ObjectFormType::Circle;
            float r = form.attribute("radius").as_float(0.f);
            out.form = r;
            return r > 0.0f;
        }
        if (type == "ellipse")
        {
            out.type = ObjectFormType::Ellipse;
            const float rx = form.attribute("rx").as_float(0.f);
            const float ry = form.attribute("ry").as_float(0.f);
            out.form = sdl3::Vector2f{rx, ry};
            return rx > 0.0f && ry > 0.0f;
        }
        if (type == "rectangle")
        {
            out.type = ObjectFormType::Rectangle;
            const float rx = form.attribute("width").as_float(0.f);
            const float ry = form.attribute("height").as_float(0.f);
            out.form = sdl3::Vector2f{rx, ry};
            return rx > 0.0f && ry > 0.0f;
        }
        if (type == "poligon8" || type == "polygon8")
        {
            out.type = ObjectFormType::Polygon8;
            std::vector<sdl3::Vector2f> vertices;
            vertices.reserve(8);

            for (const pugi::xml_node v : form.children())
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

    static bool readXml(pugi::xml_document &doc, const std::string_view configPath)
    {
        sdl3io::FileWorker read(configPath, sdl3io::FileWorkerMode::read | sdl3io::FileWorkerMode::binary);
        if (!read.isOpen())
        {
            SDL_Log("Couldn't open the file %s\n", SDL_GetError());
            return false;
        }
        std::string file = read.readAll();
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
