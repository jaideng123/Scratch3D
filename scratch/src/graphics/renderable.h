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
        unsigned int _id;
    public:
        virtual std::vector<std::shared_ptr<scratch::Mesh>> &getMeshes() = 0;

        virtual std::string getType() = 0;

        virtual const std::vector<std::shared_ptr<scratch::Material>> &getMaterials() const = 0;

        virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) = 0;

        unsigned unsigned int getId() const;
    };
}
