#include "Core.hpp"
#include "Entity.hpp"
#include <raylib.h>
#include <raymath.h>

struct WormPart {
    ES::Engine::Entity tail;
};

struct WormHead {};

struct Circle {
    Vector2 position;
    float radius;
    Color color;
};

struct Name {
    std::string name;
};

void Renderer(ES::Engine::Core &core) 
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    auto view = core.GetRegistry().view<Circle>();
    for (auto entity : view) {
        auto &circle = view.get<Circle>(entity);
        DrawCircleV(circle.position, circle.radius, circle.color); 
    }
    EndDrawing();
}

ES::Engine::Entity CreateWormPart(ES::Engine::Core &core)
{
    ES::Engine::Entity head = ES::Engine::Entity(core.GetRegistry().create());
    head.AddComponent<WormPart>(core, ES::Engine::Entity::ToEnttEntity(ES::Engine::Entity::entity_null_id));
    head.AddComponent<Circle>(core, Circle(Vector2{100.f, 100.f}, 10, BLUE));
    return head;
}

void InitWorm(ES::Engine::Core &core)
{
    auto &registry = core.GetRegistry();
    ES::Engine::Entity head = CreateWormPart(core);
    head.AddComponent<WormHead>(core);
    head.GetComponents<Circle>(core).color = RED;
    head.AddComponent<Name>(core, Name{"WormHead"});
    
    ES::Engine::Entity tail(ES::Engine::Entity::ToEnttEntity(ES::Engine::Entity::entity_null_id));
    for (int i = 0; i < 10; i++) {
        tail = CreateWormPart(core);
        tail.AddComponent<Name>(core, Name{"WormPart" + std::to_string(i)});
        head.GetComponents<WormPart>(core).tail = tail;
        head = tail;
    }
}

void InitRenderer(ES::Engine::Core &core)
{
    InitWindow(800, 600, "Worm Game");
    SetTargetFPS(60);
}

void UpdateHeadPos(ES::Engine::Core &core)
{
    core.GetRegistry().view<WormHead, Circle>().each([&](auto head, auto &circle) {
        circle.position = GetMousePosition();
    });
}

void UpdateTailPos(ES::Engine::Core &core)
{
    const float distance_between_parts = 20.f;
    core.GetRegistry().view<WormHead, WormPart, Circle>().each([&](WormPart &part, Circle &circle) {
        auto head_pos = circle.position;
        auto head_part = part;
        while (core.IsEntityValid(head_part.tail)) {
            auto &tail_circle = head_part.tail.GetComponents<Circle>(core);
            auto &tail = head_part.tail.GetComponents<WormPart>(core);

            Vector2 direction = Vector2Subtract(tail_circle.position, head_pos);
            Vector2 normalized_direction = Vector2Normalize(direction);
            Vector2 move = Vector2Scale(normalized_direction, distance_between_parts);
            tail_circle.position = Vector2Add(head_pos, move);

            head_pos = tail_circle.position;
            head_part = tail;
        }
    });
}

int main()
{
    ES::Engine::Core core;
    
    core.RegisterSystem<ES::Engine::Scheduler::Startup>(InitRenderer);
    core.RegisterSystem<ES::Engine::Scheduler::Startup>(InitWorm);

    core.RunSystems();

    core.RegisterSystem(UpdateHeadPos);
    core.RegisterSystem(UpdateTailPos);
    core.RegisterSystem(Renderer);  
    
    while (!WindowShouldClose()) {
        core.RunSystems();
    }

    return 0;
}
