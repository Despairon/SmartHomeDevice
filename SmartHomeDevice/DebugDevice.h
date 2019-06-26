#pragma once

#include <functional>
#include <string>

namespace DebugDevice_n
{
    using DebugPrintFunc = std::function<void(const std::string&)>;

    class DebugDevice
    {
    private:
        DebugDevice() = delete;

        DebugPrintFunc debugPrintFunc;
    public:
        DebugDevice(DebugPrintFunc);

        DebugDevice &operator <<(const std::string&);
        DebugDevice &operator <<(const char*);
    };
}