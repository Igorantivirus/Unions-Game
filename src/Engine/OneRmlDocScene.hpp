#pragma once

#include <stdexcept>
#include <vector>

#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/EventListener.h>

#include <Core/Managers/PathMeneger.hpp>

#include "AdvancedContext.hpp"
#include "Scene.hpp"

namespace engine
{

// Базовый класс для сцен, которые используют ровно один RmlUi документ.
class OneRmlDocScene : public Scene
{
public:
    OneRmlDocScene(Context &context, std::string_view assetRmlFile)
        : context_(context), docPath_(core::managers::PathManager::inAssets(assetRmlFile)), docId_(assetRmlFile)
    {
    }

    ~OneRmlDocScene() override
    {
        detachAllListeners();
        if (doc_)
            doc_->Hide();
        context_.closeDocument(docId_);
        doc_ = nullptr;
    }

    void hide() override
    {
        if (doc_)
            doc_->Hide();
    }
    void show() override
    {
        if (doc_)
            doc_->Show();
    }

    SceneAction update(const float dt) override
    {
        if (actionRes_.type == SceneActionType::None)
            return actionRes_;
        SceneAction res = actionRes_;
        actionRes_ = SceneAction::noneAction();
        return res;
    }

protected:
    Rml::ElementDocument *loadDocumentOrThrow()
    {
        doc_ = context_.loadIfNoDocument(docPath_, docId_);
        if (!doc_)
            throw std::logic_error("The document cannot be empty.");
        onDocumentLoaded(*doc_);
        return doc_;
    }

    Rml::ElementDocument *document() const
    {
        return doc_;
    }

    void addEventListener(const Rml::EventId eventId, Rml::EventListener *listener, const bool inCapturePhase = true)
    {
        if (!doc_)
            throw std::logic_error("Document is not loaded yet; call loadDocumentOrThrow() first.");
        doc_->AddEventListener(eventId, listener, inCapturePhase);
        listeners_.push_back(ListenerBinding{eventId, listener, inCapturePhase});
    }

    virtual void onDocumentLoaded(Rml::ElementDocument & /*doc*/)
    {
    }

protected:
    Context &context_;

private:
    struct ListenerBinding
    {
        Rml::EventId eventId{};
        Rml::EventListener *listener{};
        bool capture{};
    };

    void detachAllListeners() noexcept
    {
        if (!doc_)
        {
            listeners_.clear();
            return;
        }

        for (const auto &b : listeners_)
        {
            if (b.listener)
                doc_->RemoveEventListener(b.eventId, b.listener, b.capture);
        }
        listeners_.clear();
    }

private:
    std::string docPath_;
    std::string docId_;

    Rml::ElementDocument *doc_ = nullptr;
    std::vector<ListenerBinding> listeners_;
};
} // namespace engine