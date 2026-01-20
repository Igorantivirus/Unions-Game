#pragma once

#include <RmlUi/Debugger/Debugger.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <string>
#include <unordered_map>

#include <SDLWrapper/SDLWrapper.hpp>

#include <RmlUi/Core.h>
#include <RmlUi/FileInterface_SDL.hpp>
#include <RmlUi/RmlUi_Platform_SDL.h>
#include <RmlUi/RmlUi_Renderer_SDL.h>
#ifndef NDEBUG
#include <RmlUi/Debugger.h>
#endif

#include <Core/FileWorker.hpp>
#include <Core/Types.hpp>
#include <Core/BaseFolder.hpp>

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

    bool init(std::shared_ptr<SDL_Window> window, std::shared_ptr<SDL_Renderer> renderer, const std::string_view &fontsPath)
    {
        window_ = window;
        renderer_ = renderer;

        rendrInterface_ = std::make_unique<RenderInterface_SDL>(renderer_.get());
        systemInterface_ = std::make_unique<SystemInterface_SDL>();
        fileInterface_ = std::make_unique<FileInterface_SDL>();

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

#ifndef NDEBUG
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
        // rendrInterface_->BeginFrame();
        context_->Render();
        // rendrInterface_->EndFrame();
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

    Rml::Context* getContext()
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
#ifndef NDEBUG
    bool debugInit_ = false;
#endif

private:
    bool loadFonts(const std::string_view fontsPath)
    {
        std::string strFile;
        {
            sdl3io::FileWorker file(fontsPath, sdl3io::FileWorkerMode::read | sdl3io::FileWorkerMode::binary);
            if (!file.isOpen())
                return false;
            strFile = file.readAll();
        }
        strFile += '\n';

        std::size_t last = 0;
        for (std::size_t cur = strFile.find('\n', last); cur != std::string::npos; cur = strFile.find('\n', last))
        {
            std::string pr = (assets / strFile.substr(last, cur - last)).string();
            if (!pr.empty() && pr.back() == '\r')
                pr.pop_back();
            last = cur + 1;

            if (pr.empty())
                continue;

            SDL_Log("Next: \"%s\"", pr.c_str());
            const bool fallback = (pr.find("Emoji") != std::string::npos);
            if (!Rml::LoadFontFace(pr, fallback))
                SDL_Log("Failed to load font: %s", pr.c_str());
        }
        return true;
    }
};

} // namespace engine
