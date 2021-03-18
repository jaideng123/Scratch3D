#pragma once
#include "entity.hpp"

namespace scratch {

    class IDFactory {
    private:
        static const unsigned int MAX_ID = (255) + (255 << 8) + (255 << 16);
        unsigned int _lastGeneratedId;
    public:
        IDFactory();
        void setLastGeneratedId(unsigned int id);
        unsigned int getLastGeneratedId();

        unsigned int generate_id();
    };
}