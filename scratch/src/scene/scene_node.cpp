//
// Created by JJJai on 3/13/2021.
//

#include "scene_node.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>


glm::mat4 scratch::SceneNode::generateTransformMatrix() {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), _scale);
    glm::mat4 rotation = glm::mat4_cast(_rotation);
    return translation * rotation * scale;
}

void scratch::SceneNode::setTransform(glm::mat4 transform) {
    glm::vec3 skew = glm::vec3();
    glm::vec4 perspective = glm::vec4();
    glm::decompose(transform, _scale, _rotation, _position, skew, perspective);
}

glm::vec3 scratch::SceneNode::getPosition() {
    return _position;
}

void scratch::SceneNode::setPosition(glm::vec3 position) {
    _position = position;
}

glm::vec3 scratch::SceneNode::getScale() {
    return _scale;
}

void scratch::SceneNode::setScale(glm::vec3 scale) {
    _scale = scale;
}

glm::quat scratch::SceneNode::getRotation() {
    return _rotation;
}

void scratch::SceneNode::setRotation(glm::quat rotation) {
    _rotation = rotation;
}

void scratch::SceneNode::attachChild(std::shared_ptr<SceneNode> child) {
    _children.push_back(child);
}

void scratch::SceneNode::setParent(const std::weak_ptr<SceneNode> &parent) {
    _parent = parent;
}

const std::weak_ptr<scratch::SceneNode> &scratch::SceneNode::getParent() const {
    return _parent;
}

const std::vector<std::shared_ptr<scratch::SceneNode>> &scratch::SceneNode::getChildren() const {
    return _children;
}

