#include <SDL3/SDL_main.h>

#include <Engine/Engine.hpp>
#include <App/GameSceneFactory.hpp>

static engine::Engine app;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    auto res = app.start("Falling ellipses", "fonts/fonts.txt", {800,800});
    app.setFps(60);
    app.registrateSceneFabrick(std::make_unique<GameSceneFactory>(GameSceneFactory{}));
    app.pushScene(MainMenuScene::sceneID);
    return res;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return app.updateEvents(*event);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    return app.iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    app.close();
}

// #include <SDL3/SDL_events.h>
// #include <SDL3/SDL_keycode.h>
// #include <SDL3/SDL_log.h>

// #include <SDL3/SDL_timer.h>
// #include <SDLWrapper/Clock.hpp>
// #include <SDLWrapper/Math/Colors.hpp>
// #include <SDLWrapper/SDLWrapper.hpp>

// #include <box2d/b2_body.h>

// #include <algorithm>
// #include <unordered_set>

// #include "ContactCheker.hpp"
// #include "Entity.hpp"
// #include "EntityFactory.hpp"
// #include "deleteQueue.hpp"

// sdl3::RenderWindow window;

// b2World world(b2Vec2(0.0f, 9.81f));
// ContactCheker checker;

// std::vector<Entity> objects;

// float spd = 2.5f;

// sdl3::ClockNS cl;

// float startY = 0;

// sdl3::Color randColor()
// {
//     int num = rand() % 9;
//     switch (num)
//     {
//     case 0:
//         return sdl3::Colors::Red;
//     case 1:
//         return sdl3::Colors::Green;
//     case 2:
//         return sdl3::Colors::Blue;
//     case 3:
//         return sdl3::Colors::Yellow;
//     case 4:
//         return sdl3::Colors::Cyan;
//     case 5:
//         return sdl3::Colors::Magenta;
//     case 6:
//         return sdl3::Colors::Grey;
//     case 7:
//         return sdl3::Colors::DarkGrey;
//     case 8:
//         return sdl3::Colors::LightGrey;
//     default:
//         return sdl3::Colors::Black;
//     }
// }

// void generateGlass(const float width, const float height, const float thikness)
// {
//     auto size = window.getSize();
//     objects.push_back(EntityFactory::createRectangle(world, {size.x / 2.f, size.y * 1.f}, {width, thikness}, sdl3::Colors::Black, b2BodyType::b2_staticBody));

//     objects.push_back(EntityFactory::createRectangle(world, {size.x / 2.f - width / 2.f, size.y - height / 2.f}, {thikness, height}, sdl3::Colors::Black, b2BodyType::b2_staticBody));
//     objects.push_back(EntityFactory::createRectangle(world, {size.x / 2.f + width / 2.f, size.y - height / 2.f}, {thikness, height}, sdl3::Colors::Black, b2BodyType::b2_staticBody));

//     startY = size.y - height * (1.f + 1 / 3.f);
// }

// SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
// {
//     world.SetContactListener(&checker);

//     sdl3::VideoMode mode = sdl3::VideoMode::getDefaultVideoMode();
//     mode.fullscreen = false;
//     mode.width /= 2.f;
//     mode.height /= 2.f;
//     bool res = window.create("window", mode);
//     window.loadIconFromFile("ABS.png");

//     const sdl3::Vector2f size = {(float)mode.width, (float)mode.height};

//     // objects.push_back(EntityFactory::createEllipse(world, {350, -200}, {100, 100}, sdl3::Colors::Yellow));
//     // objects.push_back(EntityFactory::createEllipse(world, {300, 200}, {10, 10}, sdl3::Colors::Red));
//     // objects.push_back(EntityFactory::createEllipse(world, {300, 0}, {50, 10}, sdl3::Colors::Blue));
//     // objects.push_back(EntityFactory::createEllipse(world, {300, 100}, {10, 100}, sdl3::Colors::Blue));

//     generateGlass(size.x / 2.f, size.y * 2 / 3.f, 10);

//     return SDL_APP_CONTINUE;
// }

// SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
// {
//     if (event->type == SDL_EVENT_QUIT)
//     {
//         window.close();
//         return SDL_APP_SUCCESS;
//     }
//     else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
//     {
//         if (event->button.button == SDL_BUTTON_LEFT)
//         {
//             objects.push_back(EntityFactory::createEllipse(world, {event->button.x, startY}, {rand() % 20 + 10.f, rand() % 20 + 10.f}, randColor()));
//         }
//     }
//     else if (event->type == SDL_EVENT_KEY_DOWN)
//     {
//         if (event->key.key == SDLK_ESCAPE)
//         {
//             window.close();
//             return SDL_APP_SUCCESS;
//         }
//         // if (event->key.key == SDLK_N)
//         //     SDL_Log("\n");

//         // else if (event->key.key == SDLK_UP)
//         //     changed = true, obj1->shape->getShape().move({0, -spd});
//         // else if (event->key.key == SDLK_DOWN)
//         //     changed = true, obj1->shape->getShape().move({0, spd});
//         // else if (event->key.key == SDLK_LEFT)
//         //     changed = true, obj1->shape->getShape().move({-spd, 0});
//         // else if (event->key.key == SDLK_RIGHT)
//         //     changed = true, obj1->shape->getShape().move({spd, 0});

//         // else if (event->key.key == SDLK_W)
//         //     changed = true, obj2->shape->getShape().move({0, -spd});
//         // else if (event->key.key == SDLK_S)
//         //     changed = true, obj2->shape->getShape().move({0, spd});
//         // else if (event->key.key == SDLK_A)
//         //     changed = true, obj2->shape->getShape().move({-spd, 0});
//         // else if (event->key.key == SDLK_D)
//         //     changed = true, obj2->shape->getShape().move({spd, 0});

//         // else if (event->key.scancode == SDL_SCANCODE_KP_7)
//         // {
//         //     auto s = obj1->shape->getShape().getScale();
//         //     s.x /= 1.1f;
//         //     changed = true, obj1->shape->getShape().setScale(s);
//         // }
//         // else if (event->key.scancode == SDL_SCANCODE_KP_4)
//         // {
//         //     auto s = obj1->shape->getShape().getScale();
//         //     s.y /= 1.1f;
//         //     changed = true, obj1->shape->getShape().setScale(s);
//         // }
//         // else if (event->key.scancode == SDL_SCANCODE_KP_9)
//         // {
//         //     auto s = obj1->shape->getShape().getScale();
//         //     s.x *= 1.1f;
//         //     changed = true, obj1->shape->getShape().setScale(s);
//         // }
//         // else if (event->key.scancode == SDL_SCANCODE_KP_6)
//         // {
//         //     auto s = obj1->shape->getShape().getScale();
//         //     s.y *= 1.1f;
//         //     changed = true, obj1->shape->getShape().setScale(s);
//         // }
//         // else if (event->key.scancode == SDL_SCANCODE_KP_PLUS)
//         //     changed = true, obj2->shape->getShape().rotate(1);
//         // else if (event->key.scancode == SDL_SCANCODE_KP_MINUS)
//         //     changed = true, obj2->shape->getShape().rotate(-1);

//         // V2Simplex s;
//         // if (changed && CollisionMeneger::GJK(obj1->shape, obj2->shape, s))
//         // {
//         //     SDL_Log("BAN\n");
//         // }
//     }
//     return SDL_APP_CONTINUE;
// }

// SDL_AppResult SDL_AppIterate(void *appstate)
// {
//     if (!deleteQueue.empty())
//     {
//         std::unordered_set<unsigned short> ids;
//         ids.reserve(deleteQueue.size());
//         while(!deleteQueue.empty())
//         {
//             ids.insert(deleteQueue.front());
//             deleteQueue.pop();
//         }

//         if (!ids.empty())
//         {
//             objects.erase(
//                 std::remove_if(objects.begin(), objects.end(),
//                                [&](const Entity &e) { return ids.contains(e.getID()); }),
//                 objects.end());
//         }
//     }

//     // float dt = 1.f / 3600.f * 4;
//     float dt = cl.elapsedTimeS();
//     cl.start();

//     world.Step(dt, 8, 3);

//     window.clear(sdl3::Colors::White);

//     for (auto &obj : objects)
//         window.draw(obj);

//     window.display();

//     return SDL_APP_CONTINUE;
// }

// void SDL_AppQuit(void *appstate, SDL_AppResult result)
// {
//     window.close();
// }

// // #include <SDL3/SDL_keycode.h>
// // #include <SDL3/SDL_log.h>
// // #include <SDL3/SDL_main.h>

// // #include <SDL3/SDL_pixels.h>
// // #include <SDL3/SDL_timer.h>
// // #include <SDLWrapper/Clock.hpp>
// // #include <SDLWrapper/Math/Colors.hpp>
// // #include <SDLWrapper/Math/Matrix3x3.hpp>
// // #include <SDLWrapper/SDLWrapper.hpp>
// // #include <memory>

// // #include "CollisionMeneger.hpp"
// // #include "EngineConfig.hpp"
// // #include "PhysicObjects/Object.hpp"

// // sdl3::RenderWindow window;

// // // std::vector<std::unique_ptr<BaseObject>> objects_;

// // struct Deleter
// // {
// //     void operator()(Object *obj)
// //     {
// //     }
// // };

// // std::vector<Object> objects;
// // Object *obj1 = nullptr;
// // Object *obj2 = nullptr;

// // float spd = 2.5f;

// // sdl3::ClockNS cl;

// // SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
// // {
// //     // pr.reset()

// //     sdl3::VideoMode mode = sdl3::VideoMode::getDefaultVideoMode();
// //     mode.fullscreen = false;
// //     mode.width /= 2.f;
// //     mode.height /= 2.f;
// //     bool res = window.create("window", mode);
// //     window.loadIconFromFile("ABS.png");

// //     const sdl3::Vector2f size = {(float)mode.width, (float)mode.height};

// //     objects.push_back(ObjectFactory::generateEllipse(
// //         {30, 45}, {50, 0}, sdl3::Colors::Red, PhysicBody{PhysicBodyType::Dynamic}));
// //     objects.push_back(ObjectFactory::generateRectangle(
// //         {50, 30}, {100, 0}, sdl3::Colors::Blue, PhysicBody{PhysicBodyType::Dynamic}));

// //     const float wall = engine::cfg::wallThickness;
// //     objects.push_back(ObjectFactory::generateRectangle(
// //         {wall, size.y}, {0, 0}, sdl3::Colors::Black, PhysicBody{PhysicBodyType::Static}));
// //     objects.push_back(ObjectFactory::generateRectangle(
// //         {wall, size.y}, {size.x - wall, 0}, sdl3::Colors::Black, PhysicBody{PhysicBodyType::Static}));
// //     objects.push_back(ObjectFactory::generateRectangle(
// //         {size.x, wall}, {0, size.y - wall}, sdl3::Colors::Black, PhysicBody{PhysicBodyType::Static}));

// //     obj1 = &objects[0];
// //     obj2 = &objects[1];

// //     return SDL_APP_CONTINUE;
// // }

// // SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
// // {
// //     if (event->type == SDL_EVENT_QUIT)
// //     {
// //         window.close();
// //         return SDL_APP_SUCCESS;
// //     }
// //     bool changed = false;
// //     if (event->type == SDL_EVENT_KEY_DOWN)
// //     {
// //         if (event->key.key == SDLK_ESCAPE)
// //         {
// //             window.close();
// //             return SDL_APP_SUCCESS;
// //         }
// //         if (event->key.key == SDLK_N)
// //             SDL_Log("\n");

// //         else if (event->key.key == SDLK_UP)
// //             changed = true, obj1->shape->getShape().move({0, -spd});
// //         else if (event->key.key == SDLK_DOWN)
// //             changed = true, obj1->shape->getShape().move({0, spd});
// //         else if (event->key.key == SDLK_LEFT)
// //             changed = true, obj1->shape->getShape().move({-spd, 0});
// //         else if (event->key.key == SDLK_RIGHT)
// //             changed = true, obj1->shape->getShape().move({spd, 0});

// //         else if (event->key.key == SDLK_W)
// //             changed = true, obj2->shape->getShape().move({0, -spd});
// //         else if (event->key.key == SDLK_S)
// //             changed = true, obj2->shape->getShape().move({0, spd});
// //         else if (event->key.key == SDLK_A)
// //             changed = true, obj2->shape->getShape().move({-spd, 0});
// //         else if (event->key.key == SDLK_D)
// //             changed = true, obj2->shape->getShape().move({spd, 0});

// //         else if (event->key.scancode == SDL_SCANCODE_KP_7)
// //         {
// //             auto s = obj1->shape->getShape().getScale();
// //             s.x /= 1.1f;
// //             changed = true, obj1->shape->getShape().setScale(s);
// //         }
// //         else if (event->key.scancode == SDL_SCANCODE_KP_4)
// //         {
// //             auto s = obj1->shape->getShape().getScale();
// //             s.y /= 1.1f;
// //             changed = true, obj1->shape->getShape().setScale(s);
// //         }
// //         else if (event->key.scancode == SDL_SCANCODE_KP_9)
// //         {
// //             auto s = obj1->shape->getShape().getScale();
// //             s.x *= 1.1f;
// //             changed = true, obj1->shape->getShape().setScale(s);
// //         }
// //         else if (event->key.scancode == SDL_SCANCODE_KP_6)
// //         {
// //             auto s = obj1->shape->getShape().getScale();
// //             s.y *= 1.1f;
// //             changed = true, obj1->shape->getShape().setScale(s);
// //         }
// //         else if (event->key.scancode == SDL_SCANCODE_KP_PLUS)
// //             changed = true, obj2->shape->getShape().rotate(1);
// //         else if (event->key.scancode == SDL_SCANCODE_KP_MINUS)
// //             changed = true, obj2->shape->getShape().rotate(-1);

// //         V2Simplex s;
// //         if (changed && CollisionMeneger::GJK(obj1->shape, obj2->shape, s))
// //         {
// //             SDL_Log("BAN\n");
// //         }
// //     }
// //     return SDL_APP_CONTINUE;
// // }

// // SDL_AppResult SDL_AppIterate(void *appstate)
// // {
// //     float dt = cl.elapsedTimeS();
// //     cl.start();

// //     // 1) Интеграция (движение)
// //     for (auto &o : objects)
// //         o.update(dt);

// //     // 2) Коллизии (пары)
// //     for (int i = 0; i < (int)objects.size() - 1; ++i)
// //         for (int j = i + 1; j < (int)objects.size(); ++j)
// //             CollisionMeneger::EPA(objects[i], objects[j]);

// //     // for (auto &obj : objects)
// //     //     obj.update(dt);

// //     window.clear(sdl3::Colors::White);

// //     for (const auto &obj : objects)
// //         window.draw(obj);

// //     window.display();

// //     return SDL_APP_CONTINUE;
// // }

// // void SDL_AppQuit(void *appstate, SDL_AppResult result)
// // {
// //     window.close();
// // }
