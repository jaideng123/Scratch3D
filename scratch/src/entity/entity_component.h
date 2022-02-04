#pragma once

#include <include/rapidjson/prettywriter.h>
#include <string>

namespace scratch {
    class EntityComponent {
    public:
        virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) = 0;

        virtual std::string getTypeName() = 0;

        unsigned int _id;
    };
}