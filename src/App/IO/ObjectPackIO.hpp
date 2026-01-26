#pragma once

#include <filesystem>

#include <App/HardStrings.hpp>
#include <App/Resources/ObjectPack.hpp>

#include "FullFileWorker.hpp"
#include "Resources/Types.hpp"

namespace IO
{

inline bool parseFiller(const pugi::xml_node &filler, resources::ObjectFillerDef &out)
{
    if (!filler)
        return false;
    const std::string_view type = filler.attribute("type").as_string();
    if (type == "texture")
    {
        out.type = resources::ObjectFillerType::Texture;
        std::string texture = filler.attribute("texture").as_string();
        out.filler = texture;
        return !texture.empty();
    }
    if (type == "color")
    {
        out.type = resources::ObjectFillerType::Color;
        sdl3::Color color = sdl3::Color::toColor(filler.attribute("r").as_float(0.f), filler.attribute("g").as_float(0.f), filler.attribute("b").as_float(0.f));
        out.filler = color;
        return color != sdl3::Colors::Black;
    }
    return false;
}
inline bool parseForm(const pugi::xml_node &form, resources::ObjectFormDef &out)
{
    if (!form)
        return false;

    const std::string_view type = form.attribute("type").as_string();
    if (type == "circle")
    {
        out.type = resources::ObjectFormType::Circle;
        float r = form.attribute("radius").as_float(0.f);
        out.form = r;
        return r > 0.0f;
    }
    if (type == "ellipse")
    {
        out.type = resources::ObjectFormType::Ellipse;
        const float rx = form.attribute("rx").as_float(0.f);
        const float ry = form.attribute("ry").as_float(0.f);
        out.form = sdl3::Vector2f{rx, ry};
        return rx > 0.0f && ry > 0.0f;
    }
    if (type == "rectangle")
    {
        out.type = resources::ObjectFormType::Rectangle;
        const float rx = form.attribute("width").as_float(0.f);
        const float ry = form.attribute("height").as_float(0.f);
        out.form = sdl3::Vector2f{rx, ry};
        return rx > 0.0f && ry > 0.0f;
    }
    if (type == "polygon")
    {
        float sizeRatio = form.attribute("sizeRatio").as_float(1.f);
        out.type = resources::ObjectFormType::Polygon;
        std::vector<sdl3::Vector2f> vertices;

        for (const pugi::xml_node v : form.children())
        {
            const std::string name = v.name();
            if (name != "vertex")
                continue;
            const float x = v.attribute("x").as_float() * sizeRatio;
            const float y = v.attribute("y").as_float() * sizeRatio;
            vertices.push_back({x, y});
        }
        out.form = vertices;
        return vertices.size() >= 3;
    }
    return false;
}
inline bool parseSettings(const pugi::xml_node &settings, resources::PackageSettings &setts)
{
    if (!settings)
        return false;
    const pugi::xml_node objectRange = settings.child("objectRange");
    const pugi::xml_node timeStep = settings.child("timeStep");
    const pugi::xml_node death = settings.child("death");

    setts.levelRange =
        {
            objectRange.attribute("startLevel").as_uint(1),
            objectRange.attribute("endLevel").as_uint(1)};
    setts.summonTimeStepS = timeStep.attribute("seconds").as_float();
    setts.deathCount = death.attribute("count").as_uint(1);
    return true;
}

inline bool parseMusic(const pugi::xml_node &music, resources::PackageMusic &mus)
{
    if (!music)
        return false;

    const pugi::xml_node background = music.child("background");
    const pugi::xml_node win = music.child("win");
    const pugi::xml_node lose = music.child("lose");

    mus.backgroundFile = background.attribute("file").as_string();
    mus.winFile = win.attribute("file").as_string();
    mus.loseFile = lose.attribute("file").as_string();

    return true;
}

inline bool readObjectPack(resources::ObjectPack &pack, core::managers::TextureManager &textures, core::managers::AudioManager &audios, const std::string &packName, const std::filesystem::path &folderPath)
{
    pack.unload(textures, audios);

    pack.setPackageName(packName);

    const auto configFile = folderPath / assets::packagConf;

    pugi::xml_document doc;
    if (!doc.load_string(IO::readAllFile(configFile).c_str()))
        return false;

    const pugi::xml_node root = doc.child("root");
    const pugi::xml_node settings = root.child("settings");
    const pugi::xml_node music = root.child("music");
    const pugi::xml_node objects = root.child("objects");

    resources::PackageSettings setts;
    resources::PackageMusic mus;
    if (!parseSettings(settings, setts) || !parseMusic(music, mus))
        return false;
    pack.setSettings(std::move(setts));
    pack.setMusic(std::move(mus));

    std::unordered_set<std::string> loadedTextureKeys;
    std::unordered_set<std::string> loadedAudioKeys;

    for (const pugi::xml_node objectNode : objects.children("object"))
    {
        resources::ObjectDef def;

        const pugi::xml_node meta = objectNode.child("meta");
        const pugi::xml_node filler = objectNode.child("filler");
        const pugi::xml_node form = objectNode.child("form");
        const pugi::xml_node sound = objectNode.child("sound");

        def.id = objectNode.attribute("id").as_uint();
        def.level = meta.attribute("level").as_uint();
        def.points = meta.attribute("points").as_int(static_cast<int>(def.level));
        if(sound)
            def.soundFile = sound.attribute("file").as_string();

        if (!parseFiller(filler, def.filler) || !parseForm(form, def.form))
        {
            SDL_Log("Error of parseFiller or parseForm\ns");
            return false;
        }

        if (def.filler.type == resources::ObjectFillerType::Texture)
        {
            const std::string fileName = def.filler.getTextureName();
            const std::string texturePathKey = packName + '/' + fileName;
            const std::filesystem::path textureFile = folderPath / fileName;

            def.filler.filler = texturePathKey;
            if (!textures.has(texturePathKey) && loadedTextureKeys.insert(texturePathKey).second && !textures.load(texturePathKey, textureFile))
                return false;

            pack.addTextureKey(texturePathKey);
        }
        if (!def.soundFile.empty())
        {
            const std::string fileName = def.soundFile;
            const std::string audioPathKey = packName + '/' + fileName;
            const std::filesystem::path audioFile = folderPath / fileName;

            def.soundFile = audioPathKey;

            if (!audios.has(audioPathKey) && loadedAudioKeys.insert(audioPathKey).second && !audios.load(audioPathKey, audioFile))
                return false;

            pack.addAudioKey(audioPathKey);
        }
        pack.addObject(std::move(def));
    }

    return !pack.empty();
}

} // namespace IO