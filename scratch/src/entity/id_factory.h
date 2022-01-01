#pragma once

#include "entity.h"

namespace scratch {

    class IdFactory {
    private:
        static const unsigned int MAX_ID = (255) + (255 << 8) + (255 << 16);
        unsigned int _lastGeneratedId;
    public:
        IdFactory();

        void setLastGeneratedId(unsigned int id);

        unsigned int getLastGeneratedId() const;

        unsigned int generateId();
    };
}