#pragma once

#include <RmlUi/Debugger/Debugger.h>
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
        systemInterface_->SetWindow(window_.get());

        if (initialized_ = Rml::Initialise(); !initialized_)
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
    // bool updateDimensions()
    // {
    //     SDL_Window *window = window_.get();
    //     if (!window)
    //         return false;
    //     sdl3::Vector3i wSize_;
    //     if (!SDL_GetWindowSize(window, &wSize_.x, &wSize_.y))
    //     {
    //         SDL_Log("SDL_GetWindowSize failed! Error: %s", SDL_GetError());
    //         return false;
    //     }
    //     SDL_Log("Window size: %d %d\n", wSize_.x, wSize_.y);
    //     context_->SetDimensions(Rml::Vector2i(wSize_.x, wSize_.y));
    //     context_->SetDensityIndependentPixelRatio(SDL_GetWindowDisplayScale(window));
    //     return true;
    // }
    void updateEvents(const SDL_Event &constEv)
    {
        SDL_Event &ev = const_cast<SDL_Event &>(constEv);
        // SDL_Event *event = &ev;
        // if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED || event->type == SDL_EVENT_WINDOW_RESIZED || event->type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED || event->type == SDL_EVENT_DISPLAY_ORIENTATION)
        //     updateDimensions();
        // else if (event->type == SDL_EVENT_MOUSE_MOTION)
        //     context_->ProcessMouseMove(event->motion.x, event->motion.y, 0);
        // else if (event->type == SDL_EVENT_TEXT_INPUT)
        //     context_->ProcessTextInput(event->text.text);
        // else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        //     context_->ProcessMouseButtonDown(0, 0);
        // else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
        //     context_->ProcessMouseButtonUp(0, 0);
        // else if (event->type == SDL_EVENT_KEY_DOWN)
        //     context_->ProcessKeyDown(RmlSDL::ConvertKey(event->key.key), 0);
        RmlSDL::InputEventHandler(context_, window_.get(), ev);
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
        // Если документ найден
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

private:
    std::unique_ptr<RenderInterface_SDL> rendrInterface_;
    std::unique_ptr<SystemInterface_SDL> systemInterface_;
    std::unique_ptr<FileInterface_SDL> fileInterface_;

    std::shared_ptr<SDL_Window> window_;

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
            std::string pr = strFile.substr(last, cur - last);
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
