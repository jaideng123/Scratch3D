//
// Created by JJJai on 5/16/2021.
//

#include <imgui.h>
#include <iostream>
#include <string>
#include "scene_heirarchy.h"

void scratch::SceneHeirarchy::render() {
    ImGui::SetNextWindowPos(ImVec2(0, Y_OFFSET), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
    ImGui::Begin(_sceneName.c_str());
    ImGui::BeginChild("NodeList");
    static int selection = 1;
    for (auto node:_rootNode.getChildren()) {
        std::string name = node->getName() + "##" + std::to_string(node->getId());
        if (ImGui::Selectable(name.c_str(), _selectedNode == node)) {
            std::cout << "Selected: " << name << std::endl;
            _selectedNode = node;
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void scratch::SceneHeirarchy::setSelectedNode(const std::shared_ptr<scratch::SceneNode> &selectedNode) {
    _selectedNode = selectedNode;
}

const std::shared_ptr<scratch::SceneNode> &scratch::SceneHeirarchy::getSelectedNode() const {
    return _selectedNode;
}

void scratch::SceneHeirarchy::setRootNode(scratch::SceneNode &rootNode) {
    _rootNode = rootNode;
}

scratch::SceneHeirarchy::SceneHeirarchy(scratch::SceneNode &rootNode) : _rootNode(rootNode) {}

void scratch::SceneHeirarchy::setSceneName(const std::string sceneName) {
    _sceneName = sceneName;
}
