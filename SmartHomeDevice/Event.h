#pragma once

namespace EventSystem_n
{
    using EventID = unsigned char;

    class Event
    {
        EventID id;
        void   *data;
        size_t  size;
    public:
        Event();
        Event(const Event&);
        explicit Event(const EventID &id, void *data = nullptr, const size_t &size = 0);
        ~Event();
        
        Event operator=(const Event&);

        const EventID &getId() const;

        void getData(void *, const size_t&) const;
    };
}