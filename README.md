<br/>
<p align="center">
  <a href="https://github.com/SandFoxy/imgui_particles">
    <img src="hello.png" alt="Hello" width="600" height="300">
  </a>
  <br>
  <br>
  <br>
</p>

# About

Simplest implementation of particles ImGui which does not use a lot of resources

> [!NOTE]
> Particles are drawn via Window DrawList and can be overlapped with other ImGui elements.
> All functions must be called in the window to work correctly. 
> The Particles::Setup() function must be called in the window to create the initial particles.

> [!WARNING]
> The code was written in an hour so expect bugs. If you find any, I'll be glad to hear about them in Issues

Usage exemple:
```cpp
void Menu(){
        ImGui::SetNextWindowSize({ 600,600 });
        ImGui::Begin("Particles", nullptr, ImGuiWindowFlags_NoDecoration);

        Particles::Setup();//Must be called inside the window to create partials

        Particles::Draw();//Rendering particles

        ImVec2 TextSize = ImGui::CalcTextSize("Hello Particles!");
        ImGui::SetCursorPos({ 600 / 2 - (ImGui::CalcTextSize("Hello Particles!").x / 2), 600 / 2- (ImGui::CalcTextSize("Hello Particles!").y / 2) });
        ImGui::Text("Hello Particles!");

        ImGui::End();
}
```

# At the beginning of the particles.hpp file you can change some settings
