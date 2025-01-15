//
// Created by JJJai on 12/12/2020.
//

#include "id_factory.h"

scratch::IdFactory::IdFactory() {
    _lastGeneratedId = 0;
}

unsigned int scratch::IdFactory::generateId() {
    unsigned int new_id = this->_lastGeneratedId + 1;
    _lastGeneratedId = new_id;
    if (new_id > MAX_ID) {
        throw std::runtime_error("Max ID Reached");
    }
    return new_id;
}

void scratch::IdFactory::setLastGeneratedId(unsigned int id) {
    _lastGeneratedId = id;
}

unsigned int scratch::IdFactory::getLastGeneratedId() const {
    return _lastGeneratedId;
}
