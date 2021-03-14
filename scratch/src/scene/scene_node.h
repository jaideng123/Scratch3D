#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace scratch {

    class SceneNode {
    public:

        void attachChild(std::shared_ptr<SceneNode> child);

        void setParent(const std::weak_ptr<SceneNode> &parent);

        const std::weak_ptr<SceneNode> &getParent() const;

        const std::vector<std::shared_ptr<SceneNode>> &getChildren() const;

        // Transform Methods
        glm::mat4 generateTransformMatrix();

        void setTransform(glm::mat4 transform);

        glm::vec3 getPosition();

        void setPosition(glm::vec3 position);

        glm::vec3 getScale();

        void setScale(glm::vec3 scale);

        glm::quat getRotation();

        void setRotation(glm::quat rotation);
    private:
        glm::vec3 _position;
        glm::vec3 _scale;
        glm::quat _rotation;
        std::weak_ptr<SceneNode> _parent;
        std::vector<std::shared_ptr<SceneNode>> _children;
    };

}