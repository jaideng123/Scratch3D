//
// Created by JJJai on 3/13/2021.
//
#include <main.h>
#include "imgui.h"

#include "main_menu_bar.h"

void scratch::MainMenuBar::render() {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open..", "Ctrl+O")) {
            scratch::ScratchManagers->sceneManager->loadScene("./scene.json");
        }
        if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
            saveCurrentScene();
        }
        if (ImGui::MenuItem("Reload Scene", "Ctrl+R")) {
            reloadCurrentScene();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Reload Shaders")) {
        for (auto &shader : scratch::ScratchManagers->sceneManager->getShaders()) {
            shader->reload();
        }
    }

    if (ImGui::MenuItem("ImGui Demo Window")) {
        demoWindowOpen = true;
    }
    if (demoWindowOpen) {
        ImGui::ShowDemoWindow(&demoWindowOpen);
    }
    ImGui::Spacing();
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::EndMainMenuBar();
}

void scratch::MainMenuBar::saveCurrentScene() const {
    ScratchManagers->sceneManager->saveScene(
            ScratchManagers->sceneManager->getCurrentSceneFilePath());
}

void scratch::MainMenuBar::reloadCurrentScene() const {
    ScratchManagers->sceneManager->loadScene(
            ScratchManagers->sceneManager->getCurrentSceneFilePath());
}
