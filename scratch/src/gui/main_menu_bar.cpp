//
// Created by JJJai on 3/13/2021.
//
#include "imgui.h"

#include "main_menu_bar.h"

void scratch::MainMenuBar::render() {
    ImGui::BeginMainMenuBar();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndMainMenuBar();
}
