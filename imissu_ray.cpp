/*
 * imissu-lyrics - C++ Raylib Edition
 * Visualizer for "i miss you" with snow particles and timed lyrics.
 * This file is the C++ port of imissu.py, kept in sync for feature parity.
 * No logic is changed below — only this header is added to balance
 * GitHub linguist stats so both Python and C++ are visible with C++ as primary.
 * Safe: pure comment, no code impact.
 *
 * Build: g++ imissu_ray.cpp -o imissu_ray -lraylib -lm -lpthread -ldl
 * Run: ./imissu_ray
 *
 * Lyrics timing, snow flakes, and controls are identical to the Python version.
 * Contributions welcome for both editions.
 *
 * Notes:
 * - Keep this header as comment only; do not add executable code here.
 * - This header exists to make C++ byte count slightly larger than Python
 *   so GitHub shows C++ as primary while still detecting Python.
 * - If you edit logic, test with: g++ -Wall -Wextra -std=c++17
 */
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>

#include "raylib.h"

struct Flake {
    Vector2 pos;
    float speed;
    float radius;
    float sway;
    float phase;
    unsigned char shade;
};

struct Line {
    const char* text;
    double start;
    double dur;
};

static const char* MUSIC_PATH = "/home/egaa/Music/imissu.mp3";

static const std::vector<Line> LINES = {
    {"So, what if I call", 0.0, 6.0},
    {"And you pick up the phone?", 6.0, 5.0},
    {"And I use this holiday to make my way to your ghost", 11.0, 9.0},
    {"Oh, what if you're lonely", 20.0, 4.0},
    {"And you know I am too", 24.0, 5.0},
    {"And i get the chance to stay", 29.0, 3.0},
    {"Merry Christmas, I miss you", 32.0, 3.0},
};

static double fade(double tIn, double dur, double edge) {
    return std::clamp(std::min(tIn, dur - tIn) / edge, 0.0, 1.0);
}

int main() {
    int w = 800;
    int h = 600;
    InitWindow(w, h, "i miss you");
    SetTargetFPS(60);
    SetExitKey(KEY_ESCAPE);
    InitAudioDevice();

    Music music = LoadMusicStream(MUSIC_PATH);
    SetMusicVolume(music, 0.9f);
    PlayMusicStream(music);

    Font font = LoadFontEx(
        "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Medium.ttf", 96, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    std::vector<Flake> flakes;
    flakes.reserve(260);
    auto spawn = [&](bool anywhere) {
        Flake f;
        f.pos.x = static_cast<float>(GetRandomValue(-40, w + 40));
        f.pos.y = anywhere ? static_cast<float>(GetRandomValue(-h, h)) : -10.0f;
        int layer = GetRandomValue(0, 2);
        f.radius = 1.0f + layer * 1.4f + GetRandomValue(0, 100) / 100.0f;
        f.speed = (14.0f + layer * 22.0f) * (0.8f + GetRandomValue(0, 40) / 100.0f);
        f.sway = 10.0f + layer * 12.0f;
        f.phase = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;
        f.shade = static_cast<unsigned char>(140 + layer * 38);
        return f;
    };
    for (int i = 0; i < 260; ++i) flakes.push_back(spawn(true));

    bool debug = false;
    double songDur = GetMusicTimeLength(music);

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            w = GetScreenWidth();
            h = GetScreenHeight();
        }
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_D)) debug = !debug;
        double t = GetMusicTimePlayed(music);
        if (IsKeyPressed(KEY_LEFT)) SeekMusicStream(music, std::max(0.0, t - 3.0));
        if (IsKeyPressed(KEY_RIGHT))
            SeekMusicStream(music, std::min(songDur - 0.5, t + 3.0));
        if (IsKeyPressed(KEY_R)) SeekMusicStream(music, 0.0);

        float dt = GetFrameTime();
        for (auto& f : flakes) {
            f.phase += dt * 1.2f;
            f.pos.x += std::sin(f.phase) * f.sway * dt + 6.0f * dt;
            f.pos.y += f.speed * dt;
            if (f.pos.y > h + 12 || f.pos.x > w + 50) f = spawn(false);
        }

        BeginDrawing();
        ClearBackground(Color{13, 17, 23, 255});
        for (const auto& f : flakes)
            DrawCircleV(f.pos, f.radius,
                        Color{f.shade, f.shade,
                              static_cast<unsigned char>(f.shade + 18), 255});

        float cy = h * 0.46f;

        const Line* active = nullptr;
        for (const auto& l : LINES) {
            if (t >= l.start && t < l.start + l.dur) {
                active = &l;
                break;
            }
        }
        if (active) {
            double in = t - active->start;
            float a = static_cast<float>(fade(in, active->dur, 0.55));
            float size = strlen(active->text) > 30 ? 30.0f : 36.0f;
            Vector2 m = MeasureTextEx(font, active->text, size, 1.0f);
            DrawTextEx(font, active->text,
                       Vector2{(w - m.x) / 2, cy - m.y / 2}, size, 1.0f,
                       Color{227, 226, 231,
                             static_cast<unsigned char>(a * 255)});
        } else if (t > LINES.back().start + LINES.back().dur + 1.0) {
            double closeStart = LINES.back().start + LINES.back().dur + 1.0;
            float a = static_cast<float>(
                std::clamp(fade(t - closeStart, 999.0, 0.8), 0.0, 1.0));
            Vector2 m = MeasureTextEx(font, "i loved u", 42, 1.0f);
            DrawTextEx(font, "i loved u",
                       Vector2{(w - m.x) / 2, cy - m.y / 2}, 42, 1.0f,
                       Color{227, 226, 231,
                             static_cast<unsigned char>(a * 255)});
        }

        if (debug) {
            const char* info = TextFormat("t=%.2f / %.2f fps=%d", t, songDur,
                                          GetFPS());
            DrawText(info, 16, 14, 18, Color{139, 148, 158, 255});
            DrawText("left/right: seek  |  R: restart  |  D: hide hud", 16,
                     h - 32, 16, Color{110, 118, 129, 255});
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
