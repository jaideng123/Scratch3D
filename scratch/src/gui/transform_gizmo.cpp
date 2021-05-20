//
// Created by JJJai on 3/12/2021.
//

#include "imgui.h"
#include "ImGuizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "transform_gizmo.h"

void scratch::TransformGizmo::render() {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    float *matrixPointer = glm::value_ptr(_currentTransform);
    ImGuizmo::DecomposeMatrixToComponents(matrixPointer, matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrixPointer);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    glm::vec3 snap = {1.0f, 1.0f, 1.0f};
    switch (mCurrentGizmoOperation) {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap.x);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap.x);
            break;
    }
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
    glm::mat4 viewMatrix = _camera->getViewMatrix();
    glm::mat4 projectionMatrix = _camera->getProjectionMatrix();
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
                         mCurrentGizmoOperation, mCurrentGizmoMode, matrixPointer, NULL,
                         useSnap ? &snap.x : NULL);
    _currentTransform = glm::make_mat4(matrixPointer);
}

void scratch::TransformGizmo::setCurrentTransform(glm::mat4 transform) {
    _currentTransform = transform;
}

glm::mat4 scratch::TransformGizmo::getCurrentTransform() {
    return _currentTransform;
}

void scratch::TransformGizmo::setCamera(scratch::Camera *newCamera) {
    _camera = newCamera;
}

scratch::TransformGizmo::TransformGizmo(scratch::Camera *newCamera) {
    _camera = newCamera;
}
