//
// Created by JJJai on 5/18/2021.
//

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
        std::string materialName = "Material " + std::to_string(material->getId());
        if (ImGui::TreeNodeEx(materialName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            auto parameters = material->getParameters();
            int i = 0;
            for (const auto &param : parameters) {
                ++i;
                std::string propertyName = param.first;
                scratch::Parameter propertyValue = param.second;
                // TODO: remove this once model matrix is no longer part of material
                if (propertyName == "model") {
                    continue;
                }
                std::string inputValueId = "##VALUE" + propertyName;
                propertyName = renderPropertyName(material, propertyName, i);
                ImGui::PushItemWidth(150);
                switch (propertyValue.type) {
                    case scratch::ParameterType::FLOAT: {
                        float originalValue = scratch::StringConverter::parsefloat(propertyValue.value);
                        float currentValue = originalValue;
                        ImGui::InputFloat(inputValueId.c_str(), &currentValue);
                        if (currentValue != originalValue) {
                            material->setFloat(propertyName, currentValue);
                        }
                    }
                        break;
                    case scratch::ParameterType::VECTOR3: {
                        glm::vec3 originalValue = scratch::StringConverter::parsevec3(propertyValue.value);
                        glm::vec3 currentValue = originalValue;
                        ImGui::InputFloat3(inputValueId.c_str(), glm::value_ptr(currentValue));
                        if (currentValue != originalValue) {
                            material->setVec3(propertyName, currentValue);
                        }
                    }
                        break;
                    case scratch::ParameterType::BOOL: {
                        bool originalValue = scratch::StringConverter::parsebool(propertyValue.value);
                        bool currentValue = originalValue;
                        ImGui::Checkbox(inputValueId.c_str(), &currentValue);
                        if (currentValue != originalValue) {
                            material->setBool(propertyName, currentValue);
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
                                case scratch::ParameterType::FLOAT: {
                                    material->setFloat(propertyName, 0);
                                }
                                    break;
                                case scratch::ParameterType::VECTOR3: {
                                    material->setVec3(propertyName, glm::vec3(0));
                                }
                                    break;
                                case scratch::ParameterType::BOOL: {
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

            ImGui::TreePop();
            ImGui::Separator();
        }
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
