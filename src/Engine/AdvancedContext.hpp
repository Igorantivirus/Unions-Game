#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <SDLWrapper/SDLWrapper.hpp>

#include <RmlUi/Core.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Debugger/Debugger.h>
#include <RmlUi/FileInterface_SDL.hpp>
#include <RmlUi/RmlUi_Platform_SDL.h>
#include <RmlUi/RmlUi_Renderer_SDL.h>
#ifdef DEBUG_BUILD_TYPE
#include <RmlUi/Debugger.h>
#endif

#include <Core/Managers/PathMeneger.hpp>
#include <Core/StringUtils.hpp>
#include <Core/Time.hpp>
#include <Core/Types.hpp>

namespace engine
{

class Context
{
public:
    Context() = default;
    Context(const Context &other) = delete;
    Context(Context &&other) = delete;

    ~Context()
    {
        quit();
    }

    bool init(std::shared_ptr<SDL_Window> window, std::shared_ptr<SDL_Renderer> renderer, const std::filesystem::path &fontsPath)
    {
        window_ = window;
        renderer_ = renderer;

        rendrInterface_ = std::make_unique<RenderInterface_SDL>(renderer_.get());
        systemInterface_ = std::make_unique<SystemInterface_SDL>();
        fileInterface_ = std::make_unique<FileInterface_SDL>();

        rendrInterface_->SetTransformsEnabled(true);

        systemInterface_->SetWindow(window_.get());

        Rml::SetRenderInterface(rendrInterface_.get());
        Rml::SetSystemInterface(systemInterface_.get());
        Rml::SetFileInterface(fileInterface_.get());

        if (initialized_ = Rml::Initialise(); !initialized_)
        {
            SDL_Log("Rml::Initialise failed");
            return false;
        }

        // Rml::Factory::RegisterEventListenerInstancer(&eventFabrick_);

        Rml::Vector2i wSize{};
        if (!SDL_GetWindowSize(window_.get(), &wSize.x, &wSize.y))
        {
            SDL_Log("%s", SDL_GetError());
            return false;
        }
        context_ = Rml::CreateContext("main", wSize);
        if (!context_)
        {
            SDL_Log("Rml::CreateContext failed");
            return false;
        }
        context_->SetDensityIndependentPixelRatio(SDL_GetWindowDisplayScale(window_.get()));

        // Register shared data types once per UI context (not per scene/model).
        // This avoids "type already registered/bound" warnings when scenes recreate their models.
        registerRmlDataTypes_();

#ifdef DEBUG_BUILD_TYPE
        if (debugInit_ = Rml::Debugger::Initialise(context_); !debugInit_)
        {
            SDL_Log("Rml::Debugger::Initialise failed");
            return false;
        }
#endif
        return loadFonts(fontsPath);
    }

    void quit()
    {
        if (context_)
        {
            for (auto &[id, doc] : documents_)
                if (doc)
                    doc->Close();
            documents_.clear();

            auto name = context_->GetName();
            Rml::RemoveContext(name);
            context_ = nullptr;
        }
        if (initialized_)
        {
            Rml::Shutdown();
            initialized_ = false;
        }
#ifndef NDEBUG
        if (debugInit_)
        {
            Rml::Debugger::Shutdown();
            debugInit_ = false;
        }
#endif
    }

    void updateEvents(const SDL_Event &constEv)
    {
        RmlSDL::InputEventHandler(context_, window_.get(), renderer_.get(), const_cast<SDL_Event &>(constEv));
    }

    Rml::ElementDocument *loadDocument(const std::string &path, const std::string &ID)
    {
        Rml::ElementDocument *doc = context_->LoadDocument(path);
        if (!doc)
            return nullptr;
        documents_[ID] = doc;
        doc->SetId(ID);
        return doc;
    }

    Rml::ElementDocument *loadIfNoDocument(const std::string &path, const std::string &ID)
    {
        auto found = documents_.find(ID);
        if (found != documents_.end())
            return found->second;
        return loadDocument(path, ID);
    }

    Rml::ElementDocument *getDocument(const std::string &ID)
    {
        auto found = documents_.find(ID);
        return found == documents_.end() ? nullptr : found->second;
    }
    void closeDocument(const std::string &ID)
    {
        auto it = documents_.find(ID);
        if (it == documents_.end())
            return;
        if (it->second)
            it->second->Close();
        documents_.erase(it);
    }

    void render()
    {
        rendrInterface_->BeginFrame();
        context_->Render();
        rendrInterface_->EndFrame();
    }
    void update()
    {
        context_->Update();
    }

    void hideAll()
    {
        for (auto &[name, doc] : documents_)
            if (doc)
                doc->Hide();
    }

    Rml::Context *getContext()
    {
        return context_;
    }

private:
    std::unique_ptr<RenderInterface_SDL> rendrInterface_;
    std::unique_ptr<SystemInterface_SDL> systemInterface_;
    std::unique_ptr<FileInterface_SDL> fileInterface_;

    std::shared_ptr<SDL_Window> window_;
    std::shared_ptr<SDL_Renderer> renderer_;

    Rml::Context *context_ = nullptr;
    std::unordered_map<std::string, Rml::ElementDocument *> documents_;

    bool initialized_ = false;
#ifdef DEBUG_BUILD_TYPE
    bool debugInit_ = false;
#endif

private:
    void registerRmlDataTypes_()
    {
        if (rmlDataTypesRegistered_ || !context_)
            return;
        rmlDataTypesRegistered_ = true;

        Rml::DataModelConstructor constructor = context_->CreateDataModel("__engine_types");
        if (!constructor)
            return;

        constructor.RegisterScalar<core::Time>(
            [](const core::Time &t, Rml::Variant &out)
            {
                out = Rml::String(core::Time::toString(t));
            });

        // Keep the model alive for the lifetime of the context.
        engineTypesModel_ = constructor.GetModelHandle();
    }

private:
    bool rmlDataTypesRegistered_ = false;
    Rml::DataModelHandle engineTypesModel_;

private:
    bool loadFonts(const std::filesystem::path fontsPath)
    {
        std::string strFile;
        {
            sdl3::FileWorker file(fontsPath, sdl3::FileWorkerMode::read | sdl3::FileWorkerMode::binary);
            if (!file.isOpen())
                return false;
            strFile = file.readAll();
        }
        strFile += '\n';

        std::size_t last = 0;
        for (std::size_t cur = strFile.find('\n', last); cur != std::string::npos; cur = strFile.find('\n', last))
        {
            
            std::string pr = core::managers::PathManager::inAssets(core::viewSubstr(strFile, last, cur - last));
            if (!pr.empty() && pr.back() == '\r')
                pr.pop_back();
            last = cur + 1;

            if (pr.empty())
                continue;

            const bool fallback = (pr.find("Emoji") != std::string::npos);
            if (!Rml::LoadFontFace(pr, fallback))
                SDL_Log("Failed to load font: %s", pr.c_str());
        }
        return true;
    }
};

} // namespace engine
