#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <random>
#include <cmath>
#include <corecrt_math_defines.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"

#include <d3d11.h>
#include "imgui_impl_dx11.h"

#include <tchar.h>
#include <string>

class Particles_CFG {
public:
    bool draw_dots = true;
    bool draw_lines = true;

    int particles_poppulation = 100;
    float connection_distance = 100;
    int connections_ammount = 3;
    float particle_speed = 0.5f;

    ImColor dot_color = ImColor(0, 191, 255);
    float dot_radius = 3.f;

    ImColor line_color = ImColor(0, 191, 255, 100);
    float line_thickness = 1.5f;

    bool killer_mouse = true;
    float killer_mouse_radius = 15.f;
    bool ParticlesSpawned;

    Particles_CFG(
        bool draw_dots = true, //If it false then why you need this .hpp?
        bool draw_lines = true, // Draw lines from one dot to other
        bool killer_mouse = true,//Delete particle when particle in mouse area

        ImColor dot_color = ImColor(0, 191, 255), //Color Of Dots
        ImColor line_color = ImColor(0, 191, 255, 150), //Color Of Line

        float dot_radius = 3.f, //Radius of dot cirlce
        float killer_mouse_radius = 15.f, //Radius where mouse will delete particles
        float line_thickness = 1.5f, //Thickness of line

        int max_particles_poppulation = 100, //Bigger - more particles in window
        float connection_distance = 100, //Bigger - more distance for lines
        int lines_ammount_per_dot = 3, //Preatty sure you know whats this means
        float particle_speed = 0.5f //Particles movement speed
    ) : draw_dots(draw_dots), draw_lines(draw_lines), killer_mouse(killer_mouse),
        dot_color(dot_color), line_color(line_color), dot_radius(dot_radius),
        killer_mouse_radius(killer_mouse_radius), line_thickness(line_thickness),
        particles_poppulation(max_particles_poppulation),
        connection_distance(connection_distance),
        connections_ammount(lines_ammount_per_dot), particle_speed(particle_speed)
    {}
};

//-----------------

int Rand(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

bool WithinRange(float input, float reference = 50, float range = 15) {
    return std::abs(input - reference) <= range;
}

ImVec2 RandImVec(int min, int max) {
    return { (float)Rand(min, max), (float)Rand(min, max) };
}

class InternalParticle_Draw {
public:
    ImVec2 Position;
    ImVec2 Velocity;
    Particles_CFG particle_config;

    InternalParticle_Draw(Particles_CFG particle_config) {
        this->particle_config = particle_config;

        this->Position = ImVec2(Rand(0, ImGui::GetWindowSize().x), Rand(0, ImGui::GetWindowSize().y));
        float angle = Rand(0, 360) * (M_PI / 180.0f);

        this->Velocity = ImVec2(std::cos(angle) * particle_config.particle_speed, std::sin(angle) * particle_config.particle_speed);
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


class Particles {
private:
    std::vector<InternalParticle_Draw> particles;
    bool ParticlesSpawned = false;
    int respawn_clock = 0;
    int particles_to_respawn = 0;
public:
    Particles_CFG config;
    void Setup(Particles_CFG config) {
        this->config = config;
        if (!this->ParticlesSpawned) {
            for (int i = 0; i <= config.particles_poppulation; i++) {
                particles.push_back(InternalParticle_Draw(this->config));
            }
            this->ParticlesSpawned = true;
        }
    }

    void Draw() {
        for (int i = 0; i < particles.size(); i++) {
            InternalParticle_Draw& particle = particles[i];
            ImVec2 WinPos = ImGui::GetWindowPos();
            //particle.Draw();
            if (particle.OutOfWindow() || (this->config.killer_mouse && WithinRange(ImGui::GetIO().MousePos.x, WinPos.x + particle.Position.x, this->config.killer_mouse_radius) && WithinRange(ImGui::GetIO().MousePos.y, WinPos.y + particle.Position.y, this->config.killer_mouse_radius))) {
                particles.erase(particles.begin() + i);
                particles_to_respawn++;
                i--;
                continue;
            }

            if (config.draw_lines) {
                std::vector<std::pair<float, ImVec2>> distances;

                for (InternalParticle_Draw& part : particles) {
                    if (&part == &particle) continue;
                    float distance = part.GetDistance(particle.Position);
                    if (distance > config.connection_distance) continue;
                    distances.push_back(std::make_pair(distance, part.Position));
                }

                std::sort(distances.begin(), distances.end(), [](const std::pair<float, ImVec2>& a, const std::pair<float, ImVec2>& b) {
                    return a.first < b.first;
                    });


                for (int j = 0; j < config.connections_ammount && j < distances.size(); j++) {
                    ImVec2 BestPosition = distances[j].second;
                    ImGui::GetWindowDrawList()->AddLine(WinPos + BestPosition, WinPos + particle.Position, config.line_color, config.line_thickness);
                }
            }

            if (config.draw_dots) {
                ImGui::GetWindowDrawList()->AddCircleFilled(WinPos + particle.Position, config.dot_radius, config.dot_color);

                particle.Position = ImVec2(particle.Position.x + particle.Velocity.x, particle.Position.y + particle.Velocity.y);
            }
        }

        respawn_clock++;
        if (respawn_clock >= 5 && particles.size() <= this->config.particles_poppulation) {
            particles.push_back(InternalParticle_Draw(this->config));
            particles_to_respawn--;
            respawn_clock = 0;
        }
    }
};
