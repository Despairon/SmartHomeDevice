#include "DeviceParameter.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <map>

namespace SmartHomeDevice_n
{
    using namespace rapidjson;

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

    std::string DeviceParameter::typeToStr(const DeviceParamType &type)
    {
        switch (type)
        {
            case DeviceParamType::CHECKBOX: return "CHECKBOX";
            case DeviceParamType::COMBOBOX: return "COMBOBOX";
            case DeviceParamType::TEXTBOX:  return "TEXTBOX";
            default:                        return "UNKNOWN";
        }
    }

    DeviceParamType DeviceParameter::strToType(const std::string &str)
    {
        static const std::map<std::string, DeviceParamType> strToTypeMap =
        {
            {"CHECKBOX", DeviceParamType::CHECKBOX},
            {"COMBOBOX", DeviceParamType::COMBOBOX},
            {"TEXTBOX",  DeviceParamType::TEXTBOX}
        };

        if (strToTypeMap.find(str) != strToTypeMap.end())
            return strToTypeMap.at(str);
        else
            return static_cast<DeviceParamType>(-1);
    }

    std::string DeviceParameter::toJson() const
    {
        Document jsonDoc;
        
        jsonDoc.SetObject();

        Document::AllocatorType& allocator = jsonDoc.GetAllocator();

        Value _name;
        Value _type;
        Value _currentValue;
        Value _values;
        Value _readOnly;

        _name.SetString(name.c_str(), name.length(), allocator);
        
        auto typeStr = typeToStr(type);
        
        _type.SetString(typeStr.c_str(), typeStr.length(), allocator);

        _currentValue.SetString(currentValue.c_str(), currentValue.length(), allocator);

        _readOnly.SetBool(readOnly);

        _values.SetArray();
        
        for (auto val : values)
        {
            Value _val;
            
            _val.SetString(val.c_str(), val.length(), allocator);

            _values.PushBack(_val, allocator);
        }

        jsonDoc.AddMember("name",         _name,         allocator);
        jsonDoc.AddMember("type",         _type,         allocator);
        jsonDoc.AddMember("currentValue", _currentValue, allocator);
        jsonDoc.AddMember("values",       _values,       allocator);
        jsonDoc.AddMember("readOnly",     _readOnly,     allocator);

        StringBuffer buffer;

        buffer.Clear();

        Writer<StringBuffer> writer(buffer);
        jsonDoc.Accept(writer);

        return std::string(buffer.GetString());
    }

    DeviceParameter DeviceParameter::fromJson(const std::string &jsonStr)
    {
        Document jsonDoc;

        jsonDoc.Parse(jsonStr.c_str());

        bool isError = false;

        if (jsonDoc.IsObject())
        {
            const Value& nameJsonObj         = jsonDoc["name"];
            const Value& typeJsonObj         = jsonDoc["type"];
            const Value& currentValueJsonObj = jsonDoc["currentValue"];
            const Value& valuesJsonObj       = jsonDoc["values"];
            const Value& readOnlyJsonObj     = jsonDoc["readOnly"];

            if (nameJsonObj.IsString() && typeJsonObj.IsString() && currentValueJsonObj.IsString() && valuesJsonObj.IsArray() && readOnlyJsonObj.IsBool())
            {

                auto name         = std::string(nameJsonObj.GetString());
                auto type         = strToType(typeJsonObj.GetString());
                auto currentValue = std::string(currentValueJsonObj.GetString());
                auto values       = std::list<std::string>();
                auto readOnly     = readOnlyJsonObj.GetBool();

                for (SizeType i = 0; i < valuesJsonObj.Size(); i++)
                {
                    const Value& currValueInArray = valuesJsonObj[i];
                    if (currValueInArray.IsString())
                        values.push_back(std::string(currValueInArray.GetString()));

                }

                return DeviceParameter(name, type, readOnly, currentValue, values);
            }
        }

        return DeviceParameter();
    }
}