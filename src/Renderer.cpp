#include "Renderer.h"
#include "raylib.h"
#include "raygui.h"
#include "Settings.h"

Renderer::Renderer(/* args */)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SeeSynth - v0.2");
    SetTargetFPS(60);
}

Renderer::~Renderer()
{
}

void Renderer::Draw()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        //todo: implement renderer
        //DrawUi(&synth);
        //DrawSignal(&synth);
        //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        //DrawFPS(0,0);

    EndDrawing();
}