// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// This file is part of eProsima Fast DDS Visualizer Plugin.
//
// eProsima Fast DDS Visualizer Plugin is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// eProsima Fast DDS Visualizer Plugin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with eProsima Fast DDS Visualizer Plugin. If not, see <https://www.gnu.org/licenses/>.

/**
 * @file dynamic_types_utils.cpp
 */

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>

#include <nlohmann/json.hpp>

#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicDataFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeMember.hpp>
#include <fastdds/dds/xtypes/dynamic_types/TypeDescriptor.hpp>
#include <fastdds/dds/xtypes/utils.hpp>

#include "dynamic_types_utils.hpp"
#include "Exception.hpp"
#include "utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {
using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

template std::vector<std::string> get_introspection_type_names<TypeIntrospectionNumericStruct>(
        const TypeIntrospectionNumericStruct& type_names_struct);
template std::vector<std::string> get_introspection_type_names<TypeIntrospectionStringStruct>(
        const TypeIntrospectionStringStruct& type_names_struct);

template <typename T>
std::vector<std::string> get_introspection_type_names(
        const T& type_names_struct)
{
    std::vector<std::string> type_names;
    for (const auto& type_name : type_names_struct)
    {
        type_names.push_back(type_name.first);
    }
    return type_names;
}

void get_formatted_data(
        const std::string& base_type_name,
        const DataTypeConfiguration& data_type_configuration,
        TypeIntrospectionNumericStruct& numeric_data,
        TypeIntrospectionStringStruct& string_data,
        const nlohmann::json& data,
        const std::string& separator /* = "/" */)
{
    // Check if the data is numeric or string
    if (is_kind_numeric(data))
    {
        numeric_data.push_back({base_type_name, data.get<double>()});
        return;
    }
    else if (is_kind_boolean(data))
    {
        bool value = data.get<bool>();
        numeric_data.push_back({base_type_name, static_cast<double>(value)});
        return;
    }
    else if (is_kind_string(data))
    {
        string_data.push_back({base_type_name, data.get<std::string>()});
        return;
    }
    // If the data is not numeric or string, it can be null or a complex structure (array or object)
    else if (data.is_null())
    {
        // If the data is null, we do not add it to the introspection data
        return;
    }
    else if (data.is_array())
    {
        // If the data is an array, check max array size and truncate if necessary
        if (data.size() >= data_type_configuration.max_array_size)
        {
            if (data_type_configuration.discard_large_arrays)
            {
                // Discard array
                DEBUG("Discarding array " << base_type_name << " of size " << data.size());
                return;
            }
            else
            {
                // Truncate array
                DEBUG(
                    "Truncating array " << base_type_name <<
                        " of size " << data.size() <<
                        " to size " << data_type_configuration.max_array_size);
            }
        }
        for (int i = 0; i < std::min(static_cast<unsigned>(data.size()), data_type_configuration.max_array_size); i++)
        {
            get_formatted_data(
                base_type_name + "[" + std::to_string(i) + "]",
                data_type_configuration,
                numeric_data,
                string_data,
                data[i],
                separator);
        }
    }
    else if (data.is_object())
    {
        // If the data is an object, we iterate over each key
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            get_formatted_data(
                base_type_name + separator + it.key(),
                data_type_configuration,
                numeric_data,
                string_data,
                it.value(),
                separator);
        }
    }
    else
    {
        EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Data type not supported");
        return;
    }
}

bool is_kind_numeric(
        const nlohmann::json& data)
{
    return data.is_number();
}

bool is_kind_boolean(
        const nlohmann::json& data)
{
    return data.is_boolean();
}

bool is_kind_string(
        const nlohmann::json& data)
{
    return data.is_string();
}

ReturnCode_t serialize_data (
        DynamicData::_ref_type data,
        nlohmann::json& serialized_data)
{
    std::stringstream serializer_output;
    ReturnCode_t retcode;

    if (!data)
    {
        EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Data is nullptr. Skipping serialization to JSON format.");
        return RETCODE_NO_DATA;
    }

    retcode = json_serialize(data, DynamicDataJsonFormat::EPROSIMA, serializer_output);
    if (RETCODE_OK != retcode)
    {
        EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error encountered while serializing DynamicData to JSON.");
        return retcode;
    }
    serializer_output >> serialized_data;
    return RETCODE_OK;
}

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */
