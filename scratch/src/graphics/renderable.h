#pragma once

#include <vector>
#include <string>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>
#include "mesh.hpp"

namespace scratch {
    class Renderable {
    protected:
        unsigned int id;
    public:
        virtual std::vector<scratch::Mesh> &getMeshes() = 0;

        virtual std::string getType() = 0;

        virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) = 0;

        unsigned int getId() const;
    };
}
