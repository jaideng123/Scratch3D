//
// Created by JJJai on 3/13/2021.
//
#include <main.h>
#include <nfd.h>
#include <filesystem>
#include <imgui.h>

#include "main_menu_bar.h"

bool has_suffix(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void scratch::MainMenuBar::render() {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
            openSceneDialog();
        }
        if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
            saveCurrentScene();
        }
        if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
            saveSceneDialog();
        }
        if (ImGui::MenuItem("Reload Scene", "Ctrl+R")) {
            reloadCurrentScene();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Reload Shaders")) {
        scratch::ScratchManagers->shaderLibrary->reloadAllShaders();
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

void scratch::MainMenuBar::saveSceneDialog() const {
    nfdchar_t *outPath = nullptr;
    std::string currentPath = std::filesystem::current_path().string();
    nfdresult_t result = NFD_SaveDialog("json", currentPath.c_str(), &outPath);

    if (result == NFD_OKAY) {
        std::string path = outPath;
        std::cout << "Got Path: " << path << std::endl;
        if (!has_suffix(path, ".json")) {
            path += ".json";
        }
        ScratchManagers->sceneManager->saveScene(path);
    } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel" << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void scratch::MainMenuBar::openSceneDialog() const {
    nfdchar_t *outPath = nullptr;
    std::string currentPath = std::filesystem::current_path().string();
    nfdresult_t result = NFD_OpenDialog("json", currentPath.c_str(), &outPath);

    if (result == NFD_OKAY) {
        std::string path = outPath;
        std::cout << "Got Path: " << path << std::endl;
        ScratchManagers->sceneManager->loadScene(path);
    } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel" << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void scratch::MainMenuBar::saveCurrentScene() const {
    ScratchManagers->sceneManager->saveScene(
            ScratchManagers->sceneManager->getCurrentSceneFilePath());
}

void scratch::MainMenuBar::reloadCurrentScene() const {
    ScratchManagers->sceneManager->loadScene(
            ScratchManagers->sceneManager->getCurrentSceneFilePath());
}
