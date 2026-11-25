#include "clock.h"
#include "imgui.h"
#include <ctime>

void Clock::draw() {

        // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); 
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);  

        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);

        // TIME
        char time_str[16];
        std::strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

        // DATE
        char data[16];
        std::strftime(data, sizeof(data), "%d.%m.%Y", t);

        // WEEK DAY
        char day[16];
        std::strftime(day, sizeof(day), "%A", t);

        // First ImVec2 is for the min. size and the second one is for the max size
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 0), ImVec2(600, 600));
        // Set the position of the window at the start of the app
        ImGui::SetNextWindowPos(ImVec2(1000, 50), ImGuiCond_Once);
        ImGui::Begin("Time/date/day", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        // ImGui::PushFont(NULL, 36.0f); 
        ImGui::Text("%s", time_str);
        ImGui::Text("%s", data);
        ImGui::Text("%s", day);

        // ImGui::PopFont();

        ImGui::End();

        // ImGui::PopStyleVar();
        // ImGui::PopStyleColor();    
}