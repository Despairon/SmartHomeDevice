#include "DeviceParameter.h"

namespace SmartHomeDevice_n
{
    DeviceParameter::DeviceParameter(const std::string &name, const DeviceParamType &type, const bool &readOnly, const std::string &currentValue, const DeviceParamValuesList &values)
    {
        this->name         = name;
        this->type         = type;
        this->readOnly     = readOnly;
        this->currentValue = currentValue;
        this->values       = values;
    }

    const std::string &DeviceParameter::getName() const
    {
        return name;
    }

    const DeviceParamValuesList &DeviceParameter::getValues() const
    {
        return values;
    }

    const DeviceParamType &DeviceParameter::getType() const
    {
        return type;
    }

    const std::string &DeviceParameter::getCurrentValue() const
    {
        return currentValue;
    }

    const bool &DeviceParameter::isReadOnly() const
    {
        return readOnly;
    }

    void DeviceParameter::setName(const std::string &name)
    {
        this->name = name;
    }
    void DeviceParameter::addValue(const std::string &value)
    {
        values.push_back(value);
    }

    void DeviceParameter::setType(const DeviceParamType &type)
    {
        this->type = type;
    }

    void DeviceParameter::setCurrentValue(const std::string &currentValue)
    {
        this->currentValue = currentValue;
    }

    void DeviceParameter::setReadOnly(const bool &readOnly)
    {
        this->readOnly = readOnly;
    }
}