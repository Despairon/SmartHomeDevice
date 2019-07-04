#pragma once

#include <string>
#include <list>

namespace SmartHomeDevice_n
{
    namespace DeviceParameterType
    {
        enum Values: unsigned char
        {
            CHECKBOX,
            COMBOBOX,
            TEXTBOX
        };
    };
   
    using DeviceParamValuesList = std::list<std::string>;
    using DeviceParamType = DeviceParameterType::Values;

    class DeviceParameter
    {
    private:
        std::string           name;
        DeviceParamType       type;
        std::string           currentValue;
        DeviceParamValuesList values;
        bool                  readOnly;

    public:
        DeviceParameter() = default;
        DeviceParameter(const std::string&, const DeviceParamType&, const bool&, const std::string& = std::string(""), const DeviceParamValuesList& = DeviceParamValuesList());

        const std::string &getName() const;
        const DeviceParamValuesList &getValues() const;
        const DeviceParamType &getType() const;
        const std::string &getCurrentValue() const;
        const bool &isReadOnly() const;

        void setName(const std::string&);
        void addValue(const std::string&);
        void setType(const DeviceParamType&);
        void setCurrentValue(const std::string&);
        void setReadOnly(const bool&);

        static std::string typeToStr(const DeviceParamType&);
        static DeviceParamType strToType(const std::string&);

        std::string toJson() const;
        static DeviceParameter fromJson(const std::string&);
    };
}