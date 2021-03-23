//
// Created by JJJai on 7/11/2020.
//

#ifndef SCRATCH_RENDERABLE_H
#define SCRATCH_RENDERABLE_H


#include <vector>
#include <string>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>


namespace scratch { class Mesh; }


namespace scratch {
    class Renderable {
    protected:
        unsigned int Id;
    public:
        virtual std::vector<scratch::Mesh> &getMeshes() = 0;

        virtual std::string getType() = 0;

        virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) = 0;

        unsigned int getId() const;
    };
}


#endif //SCRATCH_RENDERABLE_H
