#include "Renderer.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
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

void Renderer::Draw(Synth& synth, const SynthGuiState& synthGui)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        //todo: implement renderer
        DrawUi(synth, synthGui);
        DrawSignal(synth, synthGui);
        //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        //DrawFPS(0,0);

    EndDrawing();
}

void Renderer::DrawSignal(Synth & synth, const SynthGuiState & synthGui) 
{
    GuiGrid((Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, "", WINDOW_HEIGHT / 8, 2);
    auto signal = synth.GetOutSignal();
    Vector2* signal_points = new Vector2[signal.size()];
    const float screen_vertical_midpoint = (WINDOW_HEIGHT/2);
    for (int point_idx = 0; point_idx < signal.size(); point_idx++)
    {
        signal_points[point_idx].x = (float)point_idx + OSCILLATOR_PANEL_WIDTH;
        signal_points[point_idx].y = screen_vertical_midpoint + (int)(signal[point_idx] * 300);
    }
    DrawLineStrip(signal_points, signal.size(), RED);
    delete[] signal_points;
}

void Renderer::DrawUi(Synth & synth, const SynthGuiState & synthGui)
{

}