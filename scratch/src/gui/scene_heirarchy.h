#pragma once

#include <scene/scene_node.h>

namespace scratch {

    class SceneHeirarchy {
    public:
        SceneHeirarchy(SceneNode &rootNode);

        void render();

        void setSelectedNode(const std::shared_ptr<scratch::SceneNode> &selectedNode);

        const std::shared_ptr<scratch::SceneNode> &getSelectedNode() const;

        void setRootNode(SceneNode &rootNode);

    private:
        constexpr static const float Y_OFFSET = 30.0f;
        std::shared_ptr<scratch::SceneNode> _selectedNode;
        scratch::SceneNode &_rootNode;

    };

}