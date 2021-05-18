//
// Created by JJJai on 5/18/2021.
//

#include <imgui.h>
#include "material_props_widget.h"

void scratch::MaterialPropsWidget::render() {
    for (const auto &material : _materials) {
        std::string materialName = "Material " + std::to_string(material->getId());
        if (ImGui::TreeNodeEx(materialName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (auto param : material->getParameters()) {
                // TODO: remove this once model matrix is no longer part of material
                if (param.first == "model") {
                    continue;
                }
                if (param.second.type == scratch::ParameterType::FLOAT) {
                    float originalValue = scratch::StringConverter::parsefloat(param.second.value);
                    float currentValue = originalValue;
                    ImGui::InputFloat(param.first.c_str(), &currentValue);
                    if (currentValue != originalValue) {
                        material->setFloat(param.first, currentValue);
                    }
                }
            }
            ImGui::TreePop();
            ImGui::Separator();
        }
    }
}

void scratch::MaterialPropsWidget::setMaterials(const std::vector<std::shared_ptr<Material>> &materials) {
    _materials = materials;
}
