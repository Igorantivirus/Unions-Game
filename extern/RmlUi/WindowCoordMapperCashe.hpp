#pragma once

#include <RmlUi/Core/Math.h>
#include <RmlUi/Core/Types.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

struct WindowCoordMapperCashe
{

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    float pixel_density = 1.f;
    int logical_w = 0;
    int logical_h = 0;
    bool logical_enabled = false;

    bool dirty = true;

    void markDirty()
    {
        dirty = true;
    }

    void update(SDL_Window *in_window, SDL_Renderer *in_renderer)
    {
        if (!dirty && window == in_window && renderer == in_renderer)
            return;

        window = in_window;
        renderer = in_renderer;

#if SDL_MAJOR_VERSION >= 3
        pixel_density = SDL_GetWindowPixelDensity(window);
#else
        pixel_density = 1.f;
#endif

        SDL_RendererLogicalPresentation mode{};
        if (!SDL_GetRenderLogicalPresentation(renderer, &logical_w, &logical_h, &mode))
        {
            logical_w = 0;
            logical_h = 0;
        }

        logical_enabled = (logical_w > 0 && logical_h > 0);
        dirty = false;
    }

    Rml::Vector2i getRmlDimensions(SDL_Window *in_window, SDL_Renderer *in_renderer, int pixel_w_hint, int pixel_h_hint)
    {
        update(in_window, in_renderer);

        if (logical_enabled)
            return {logical_w, logical_h};

        if (pixel_w_hint > 0 && pixel_h_hint > 0)
            return {pixel_w_hint, pixel_h_hint};

        int w_points = 0, h_points = 0;
        if (SDL_GetWindowSize(in_window, &w_points, &h_points))
            return {int(w_points * pixel_density), int(h_points * pixel_density)};

        return {};
    }

    bool windowToRml(SDL_Window *in_window, SDL_Renderer *in_renderer, float window_x, float window_y, float &out_x, float &out_y)
    {
        update(in_window, in_renderer);

        // #if SDL_MAJOR_VERSION >= 3
        //         if (logical_enabled)
        //         {
        //             float rx = 0.f, ry = 0.f;
        //             SDL_RenderCoordinatesFromWindow(renderer, window_x, window_y, &rx, &ry);
        //             out_x = rx;
        //             out_y = ry;
        //             return true;
        //         }
        // #endif

        out_x = window_x * pixel_density;
        out_y = window_y * pixel_density;
        return true;
    }
};