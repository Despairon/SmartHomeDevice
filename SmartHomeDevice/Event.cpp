#include <memory>

#include "Event.h"

namespace EventSystem_n
{
    Event::Event()
    {
        id   = 0;
        data = nullptr;
        size = 0;
    }

    Event::Event(const Event &other)
    {
        id = other.id;

        if ( (other.data != nullptr) && (other.size != 0) )
        {
            data = malloc(other.size);
            memcpy(data, other.data, other.size);
            size = other.size;
        }
        else
        {
            data = nullptr;
            size = 0;
        }
    }

    Event::Event(const EventID &id, void *data, const size_t &size)
    {
        this->id = id;

        if ((data != nullptr) && (size != 0))
        {
            this->data = malloc(size);
            memcpy(this->data, data, size);
            this->size = size;
        }
        else
        {
            this->data = nullptr;
            this->size = 0;
        }
    }

    Event::~Event()
    {
        if ((data != nullptr) && (size != 0))
        {
            id = 0;
            free(data);
            data = nullptr;
            size = 0;
        }
    }

    Event Event::operator=(const Event &other)
    {
        if ((other.data != nullptr) && (other.size != 0))
        {
            data = malloc(other.size);
            memcpy(data, other.data, other.size);
            size = other.size;
        }
        else
        {
            data = nullptr;
            size = 0;
        }

        return *this;
    }

    const EventID &Event::getId() const
    {
        return id;
    }

    void Event::getData(void *buffer, const size_t &bufSize) const
    {
        if ((data != nullptr) && (size != 0) && (buffer != nullptr) && (bufSize >= size))
        {
            memcpy(buffer, data, size);
        }
    }
}