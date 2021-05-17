//
// Created by JJJai on 5/16/2021.
//

#include <imgui.h>
#include <iostream>
#include <string>
#include "scene_heirarchy.h"

void scratch::SceneHeirarchy::render() {
    ImGui::SetNextWindowPos(ImVec2(1000, Y_OFFSET), ImGuiCond_Once);
    ImGui::Begin("Scene Hierarchy");
    ImGui::BeginChild("Scrolling");
    static int selection = 1;
    for (int n = 0; n < 5; n++) {
        std::string name = "Object##" + std::to_string(n);
        if (ImGui::Selectable(name.c_str(), selection == n)) {
            std::cout << "Selected: " << n << std::endl;
            selection = n;
        }
    }
    ImGui::EndChild();
    ImGui::End();
}
