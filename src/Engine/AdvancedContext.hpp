#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <SDLWrapper/SDLWrapper.hpp>

#include <RmlUi/Core.h>
#include <RmlUi/FileInterface_SDL.hpp>
#include <RmlUi/RmlUi_Backend.h>
#include <RmlUi/RmlUi_Platform_SDL.h>
#include <RmlUi/RmlUi_Renderer_SDL.h>
#ifndef NDEBUG
#include <RmlUi/Debugger.h>
#endif

#include <Core/FileWorker.hpp>
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

    bool init(sdl3::RenderWindow &window, const std::string_view &fontsPath)
    {
        window_ = window.getNativeSDLWindow();

        rendrInterface_ = std::make_unique<RenderInterface_SDL>(window.getNativeSDLRenderer().get());
        systemInterface_ = std::make_unique<SystemInterface_SDL>();
        fileInterface_ = std::make_unique<FileInterface_SDL>();

        Rml::SetRenderInterface(rendrInterface_.get());
        Rml::SetSystemInterface(systemInterface_.get());
        Rml::SetFileInterface(fileInterface_.get());

        if (!Rml::Initialise())
        {
            SDL_Log("Rml::Initialise failed");
            return false;
        }

        // Rml::Factory::RegisterEventListenerInstancer(&eventFabrick_);

        const auto wSize = window.getSize();
        context_ = Rml::CreateContext("main", Rml::Vector2i(wSize.x, wSize.y));
        if (!context_)
        {
            SDL_Log("Rml::CreateContext failed");
            return false;
        }
        context_->SetDensityIndependentPixelRatio(window.getDisplayScale());

#ifndef NDEBUG
        Rml::Debugger::Initialise(context_);
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
        Rml::Shutdown();
    }

    void updateEvents(const SDL_Event &event)
    {
        SDL_Event &ev = const_cast<SDL_Event &>(event);
        RmlSDL::InputEventHandler(context_, window_.get(), ev);
    }

    Rml::ElementDocument *loadDocument(const std::string &path, const std::string &ID)
    {
        Rml::ElementDocument *doc = context_->LoadDocument(path);
        if (!doc)
            return nullptr;
        documents_[ID] = doc;
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
        // Если документ найден
        if (it->second)
            it->second->Close();
        documents_.erase(it);
    }

    void render()
    {
        rendrInterface_->BeginFrame();
        context_->Render();
        // EndFrame для SDL не нужен, он ничего не делает
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

private:
    std::unique_ptr<RenderInterface_SDL> rendrInterface_;
    std::unique_ptr<SystemInterface_SDL> systemInterface_;
    std::unique_ptr<FileInterface_SDL> fileInterface_;

    std::shared_ptr<SDL_Window> window_;

    Rml::Context *context_ = nullptr;
    std::unordered_map<std::string, Rml::ElementDocument *> documents_;

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

        for (std::size_t cur = strFile.find('\n'), last = 0; cur != std::string::npos; last = cur, cur = strFile.find('\n', last + 1))
        {
            std::string pr = strFile.substr(last, cur - last);
            SDL_Log("Next: \"%s\"", pr.c_str());
            bool fallback = (pr.find("Emoji") != std::string::npos);
            if (!Rml::LoadFontFace(pr, fallback))
                SDL_Log("Failed to load font: %s", pr.c_str());
        }
        return true;
    }
};

} // namespace engine
