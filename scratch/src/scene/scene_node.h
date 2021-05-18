#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include "entity/entity.hpp"

namespace scratch {

    class SceneNode {
    public:
        SceneNode();

        void attachChild(std::shared_ptr<SceneNode> child);

        void setParent(const std::weak_ptr<SceneNode> &parent);

        const std::weak_ptr<SceneNode> &getParent() const;

        const std::vector<std::shared_ptr<SceneNode>> &getChildren() const;

        void addChild(const std::shared_ptr<scratch::SceneNode> &child);

        const std::shared_ptr<scratch::Entity> &getEntity() const;

        void setEntity(const std::shared_ptr<scratch::Entity> &entity);

        // Transform Methods
        glm::mat4 generateTransformMatrix();

        void setTransform(glm::mat4 transform);

        glm::vec3 getPosition();

        void setPosition(glm::vec3 position);

        glm::vec3 getScale();

        void setScale(glm::vec3 scale);

        glm::quat getRotation();

        void setRotation(glm::quat rotation);

        unsigned int getId() const;

        void setId(unsigned int id);

        const std::string &getName() const;

        void setName(const std::string &name);

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void
        deserialize(const rapidjson::Value &object, const std::vector<std::shared_ptr<scratch::Entity>> &entities);


    private:
        glm::vec3 _position;
        glm::vec3 _scale;
        glm::quat _rotation;
        std::weak_ptr<SceneNode> _parent;
        std::vector<std::shared_ptr<SceneNode>> _children;
        std::shared_ptr<scratch::Entity> _entity;
        std::string _name;
        unsigned int _id;
    };

}