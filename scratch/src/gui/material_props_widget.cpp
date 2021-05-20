//
// Created by JJJai on 5/18/2021.
//

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
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
            for (const auto &param : parameters) {
                // TODO: remove this once model matrix is no longer part of material
                if (param.first == "model") {
                    continue;
                }
                std::string inputValueId = "##VALUE" + param.first;
                std::string inputNameId = "##NAME";

                switch (param.second.type) {
                    case scratch::ParameterType::FLOAT: {
                        float originalValue = scratch::StringConverter::parsefloat(param.second.value);
                        float currentValue = originalValue;
                        std::string currentName = param.first;
                        ImGui::InputText(inputNameId.c_str(), &currentName);
                        if (currentName != param.first) {
                            material->renameParameter(param.first, currentName);
                        }
                        ImGui::PushItemWidth(100);
                        ImGui::InputFloat(inputValueId.c_str(), &currentValue);
                        ImGui::PopItemWidth();
                        if (currentValue != originalValue) {
                            material->setFloat(currentName, currentValue);
                        }
                    }
                }
                ImGui::SameLine();
                std::string currentType = PARAM_TYPE_TO_STRING.find(param.second.type)->second;
                std::string comboBoxId = "##TYPESELECT-" + param.first;
                ImGui::PushItemWidth(100);
                if (ImGui::BeginCombo(comboBoxId.c_str(), currentType.c_str())) {
                    for (auto const &type: STRING_TO_PARAM_TYPE) {
                        std::string typeName = type.first;
                        if (ImGui::Selectable(typeName.c_str(), typeName == currentType)) {
                            // handle selection
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    material->removeParameter(param.first);
                }
            }
            if (ImGui::Button("+")) {
                // TODO: create New Property
            }

            ImGui::TreePop();
            ImGui::Separator();
        }
    }
}

void scratch::MaterialPropsWidget::setMaterials(const std::vector<std::shared_ptr<Material>> &materials) {
    _materials = materials;
}
