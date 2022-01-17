//
// Created by JJJai on 5/18/2021.
//
#include "main.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <utilities/assert.h>
#include "material_props_widget.h"

void scratch::MaterialPropsWidget::render() {
    std::vector<std::string> paramTypes{};
    for (auto const &param: STRING_TO_PARAM_TYPE) {
        paramTypes.push_back(param.first);
    }
    for (const auto &material : _materials) {
        std::string materialName = material->getName();
        if (ImGui::TreeNodeEx(materialName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            std::vector<std::string> shaders = scratch::ScratchManagers->shaderLibrary->getLoadedShaderNames();
            std::string currentShaderName = material->getShader()->getName();
            if (ImGui::BeginCombo("shader", currentShaderName.c_str())) {
                for (auto const &shader: shaders) {
                    if (ImGui::Selectable(shader.c_str(), shader == currentShaderName)) {
                        material->setShader(scratch::ScratchManagers->shaderLibrary->findShader(shader));
                    }
                }
                ImGui::EndCombo();
            }
            renderProperties(material);
        }
        ImGui::TreePop();
        ImGui::Separator();
    }
}

void scratch::MaterialPropsWidget::renderProperties(const std::shared_ptr<Material> &material) const {
    auto parameters = material->getParameters();
    int i = 0;
    for (const auto &param : parameters) {
        ++i;
        std::string propertyName = param.first;
        Parameter propertyValue = param.second;
        std::string inputValueId = "##VALUE" + propertyName;
        propertyName = renderPropertyName(material, propertyName, i);
        ImGui::PushItemWidth(150);
        switch (propertyValue.type) {
            case BOOL: {
                bool originalValue = std::get<bool>(propertyValue.value);
                bool currentValue = originalValue;
                ImGui::Checkbox(inputValueId.c_str(), &currentValue);
                if (currentValue != originalValue) {
                    material->setBool(propertyName, currentValue);
                }
            }
                break;
            case FLOAT: {
                float originalValue = std::get<float>(propertyValue.value);
                float currentValue = originalValue;
                ImGui::InputFloat(inputValueId.c_str(), &currentValue);
                if (currentValue != originalValue) {
                    material->setFloat(propertyName, currentValue);
                }
            }
                break;
            case VECTOR3: {
                glm::vec3 originalValue = std::get<glm::vec3>(propertyValue.value);
                glm::vec3 currentValue = originalValue;
                ImGui::InputFloat3(inputValueId.c_str(), glm::value_ptr(currentValue));
                if (currentValue != originalValue) {
                    material->setVec3(propertyName, currentValue);
                }
            }
                break;
            default:
            SCRATCH_ASSERT_NEVER("Unknown Param Type");
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        std::string currentType = PARAM_TYPE_TO_STRING.find(propertyValue.type)->second;
        std::string comboBoxId = "##TYPESELECT-" + propertyName;
        ImGui::PushItemWidth(100);
        if (ImGui::BeginCombo(comboBoxId.c_str(), currentType.c_str())) {
            for (auto const &type: STRING_TO_PARAM_TYPE) {
                std::string typeName = type.first;
                if (ImGui::Selectable(typeName.c_str(), typeName == currentType)) {
                    switch (type.second) {
                        case FLOAT: {
                            material->setFloat(propertyName, 0);
                        }
                            break;
                        case VECTOR3: {
                            material->setVec3(propertyName, glm::vec3(0));
                        }
                            break;
                        case BOOL: {
                            material->setBool(propertyName, false);
                        }
                            break;
                        default:
                        SCRATCH_ASSERT_NEVER("Unknown Param Type");
                    }
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("-")) {
            material->removeParameter(propertyName);
        }
    }
    if (ImGui::Button("+")) {
        std::string newPropertyName = "Property " + std::to_string(material->getParameters().size() + 1);
        material->setFloat(newPropertyName, 0);
    }
}

std::string scratch::MaterialPropsWidget::renderPropertyName(const std::shared_ptr<Material> &material,
                                                             const std::string &propertyName,
                                                             size_t propertyIndex) const {
    // TODO: fix this!!!!!
    std::string inputNameId = "##PROPERTYNAME-" + std::to_string(propertyIndex);
    std::string currentName = propertyName;
    ImGui::InputText(inputNameId.c_str(), &currentName);
    if (currentName != propertyName) {
        material->renameParameter(propertyName, currentName);
    }
    return currentName;
}

void scratch::MaterialPropsWidget::setMaterials(const std::vector<std::shared_ptr<Material>> &materials) {
    _materials = materials;
}
