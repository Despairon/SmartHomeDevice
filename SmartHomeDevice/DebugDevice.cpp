#include "DebugDevice.h"

namespace DebugDevice_n
{
    DebugDevice::DebugDevice(DebugPrintFunc debugPrintFunc)
    {
        this->debugPrintFunc = debugPrintFunc;
    }

    DebugDevice &DebugDevice::operator<<(const std::string &debugMessage)
    {
        if (debugPrintFunc != nullptr)
            debugPrintFunc(debugMessage);

        return *this;
    }

    DebugDevice &DebugDevice::operator<<(const char *debugMessage)
    {
        if ( (debugPrintFunc != nullptr) && (debugMessage != nullptr) )
            debugPrintFunc(std::string(debugMessage));

        return *this;
    }
}