//
// Created by JJJai on 3/13/2021.
//
#include <main.h>
#include "imgui.h"

#include "main_menu_bar.h"

void scratch::MainMenuBar::render() {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open..", "Ctrl+O")) { scratch::ScratchManagers->sceneManager->loadScene("./scene.json"); }
        if (ImGui::MenuItem("Save", "Ctrl+S")) { scratch::ScratchManagers->sceneManager->saveScene("./scene.json"); }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Reload Shaders", "Ctrl+R")) {
        for (auto &shader : scratch::ScratchManagers->sceneManager->getShaders()) {
            shader->reload();
        }
    }
    ImGui::Spacing();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndMainMenuBar();
}
