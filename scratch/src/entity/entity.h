#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include "graphics/model.h"
#include "entity_component.h"


namespace scratch {
    class Scene;

    class Entity {
    public:
        Entity(unsigned int id, Scene *scene);

        unsigned int getID() const {
            return _id;
        }

        template<typename T>
        void removeComponent() {
            auto to_remove = std::find(_components.cbegin(), _components.cend(),
                                       [&](auto *component) {
                                           return std::dynamic_pointer_cast<T>(component) != nullptr;
                                       });
            if (to_remove != _components.cend()) {
                _components.erase(to_remove);
            } else {
                SCRATCH_ASSERT_NEVER("Attempted to remove component that doesn't exist");
            }
        }

        template<typename T>
        bool hasComponent() const {
            for (auto &component: _components) {
                if (std::dynamic_pointer_cast<T>(component) != nullptr) {
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        std::shared_ptr<T> getComponent() {
            for (auto &component: _components) {
                if (std::dynamic_pointer_cast<T>(component) != nullptr) {
                    return std::dynamic_pointer_cast<T>(component);
                }
            }
            return nullptr;
        }

        template<typename T, typename... Args>
        std::shared_ptr<T> addComponent(Args &&... args) {
            // We only allow 1 component per type for now
            SCRATCH_ASSERT(!hasComponent<T>());
            std::shared_ptr<T> new_component = std::make_shared<T>(std::forward<Args>(args)...);
            _components.push_back(new_component);
            return new_component;
        }

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

    private:
        unsigned int _id;
        Scene *_sceneRef;
        std::vector<std::shared_ptr<scratch::EntityComponent>> _components;
    };
}
