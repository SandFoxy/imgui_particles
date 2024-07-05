#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <random>
#include <cmath>
#include <corecrt_math_defines.h>
#include <string>

//Particles thats renders in a window!
//Fully developed by SandFoxy in 56 minutes!

//Particles Settings

bool debug_performance = false;// Will show current FPS, dots poppulation and ammount of connections

bool draw_dots = true;
bool draw_lines = true;

int particles_poppulation = 100;
float connection_distance = 100;
int connections_ammount = 3;
float particle_speed = 1.0f;

ImColor dot_color = ImColor(0, 191, 255);
float dot_radius = 3.f;

ImColor line_color = ImColor(0, 191, 255, 100);
float line_thickness = 1.5f;

//-----------------

int Rand(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

ImVec2 RandImVec(int min, int max) {
    return { (float)Rand(min,max),(float)Rand(min,max) };
}

class Particle {
public:
    ImVec2 Position;
    ImVec2 Velocity;

    Particle() {

        this->Position = ImVec2(Rand(0, ImGui::GetWindowSize().x), Rand(0, ImGui::GetWindowSize().y));
        float angle = Rand(0, 360) * (M_PI / 180.0f);
        
        this->Velocity = ImVec2(std::cos(angle) * particle_speed, std::sin(angle) * particle_speed);
    }

    void Draw() {
        ImVec2 WinPos = ImGui::GetWindowPos();

        if (draw_dots) ImGui::GetWindowDrawList()->AddCircleFilled(WinPos + Position, dot_radius, dot_color);

        this->Position = ImVec2(this->Position.x + this->Velocity.x, this->Position.y + this->Velocity.y);
    }

    float GetDistance(const ImVec2& p1) {
        float dx = this->Position.x - p1.x;
        float dy = this->Position.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    bool OutOfWindow() {
        ImVec2 point = this->Position;
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 pointScreenPos = ImVec2(point.x + windowPos.x, point.y + windowPos.y);

        if (pointScreenPos.x < windowPos.x || pointScreenPos.x > windowPos.x + windowSize.x ||
            pointScreenPos.y < windowPos.y || pointScreenPos.y > windowPos.y + windowSize.y) {
            return true;
        }

        return false;
    }
};


namespace Particles {

    std::vector<Particle> particles;
    bool setuped;

    void Setup() {
        if (!setuped) {
            for (int i = 0; i < particles_poppulation; i++) {
                particles.push_back(Particle());
            }
            setuped = true;
        }
    }

    void Draw() {
        int connections = 0;
        for (int i = 0; i < particles.size(); i++) {
            Particle& particle = particles[i];
            particle.Draw();
            if (particle.OutOfWindow()) {
                particles.erase(particles.begin() + i);
                particles.push_back(Particle());
                i--;
                continue;
            }

            if (draw_lines) {
                std::vector<std::pair<float, ImVec2>> distances;

                for (Particle& part : particles) {
                    if (&part == &particle) continue;
                    float distance = part.GetDistance(particle.Position);
                    if (distance > connection_distance) continue;
                    distances.push_back(std::make_pair(distance, part.Position));
                }

                std::sort(distances.begin(), distances.end(), [](const std::pair<float, ImVec2>& a, const std::pair<float, ImVec2>& b) {
                    return a.first < b.first;
                    });

                ImVec2 WinPos = ImGui::GetWindowPos();
                for (int j = 0; j < connections_ammount && j < distances.size(); j++) {
                    if (debug_performance)
                        connections++;
                    ImVec2 BestPosition = distances[j].second;
                    ImGui::GetWindowDrawList()->AddLine(WinPos + BestPosition, WinPos + particle.Position, line_color, line_thickness);
                }
            }
        }

        if (debug_performance) {
            ImVec2 WinPos = ImGui::GetWindowPos();
            ImGui::GetWindowDrawList()->AddText(WinPos,ImColor(255,255,255), std::string("FPS: " + std::to_string(ImGui::GetIO().Framerate)).c_str());
            ImGui::GetWindowDrawList()->AddText(WinPos + ImVec2(0,12), ImColor(255, 255, 255), std::string("There is " + std::to_string(particles.size()) + " particles out of " + std::to_string(particles_poppulation)).c_str());
            ImGui::GetWindowDrawList()->AddText(WinPos + ImVec2(0, 24), ImColor(255, 255, 255), std::string("There is " + std::to_string(connections) + " connections").c_str());

        }
    }
}
