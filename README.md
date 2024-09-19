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
> The  function SHOUD be called in the window to create the initial particles.

## Features:
> Basic Particles System ✅
> Killer-Mouse System ✅
> Lines Warping 🕙

Usage exemple:
```cpp
void Menu(){
        ImGui::SetNextWindowSize({ 600,600 });

        ImGui::Begin("Hello, Particles!");
        static Particles particles;

        static config =  Particles_CFG(
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
       )

        particles.Setup(config);//Must be called inside the window to create partials!!!
        particles.Draw();//Rendering particles

        //Also you can update config at any time!
        //particles.config = config;

        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();
}
```
