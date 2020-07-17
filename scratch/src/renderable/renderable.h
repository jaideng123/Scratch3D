//
// Created by JJJai on 7/11/2020.
//

#ifndef SCRATCH_RENDERABLE_H
#define SCRATCH_RENDERABLE_H


#include <vector>
#include <mesh/mesh.hpp>

namespace scratch {
    class Renderable {
    public:
        virtual std::vector<scratch::Mesh> &getMeshes() = 0;
    };
}


#endif //SCRATCH_RENDERABLE_H
