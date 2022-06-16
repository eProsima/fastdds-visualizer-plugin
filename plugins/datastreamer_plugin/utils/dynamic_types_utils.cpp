// Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// This file is part of eProsima Fast DDS Monitor.
//
// eProsima Fast DDS Monitor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// eProsima Fast DDS Monitor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with eProsima Fast DDS Monitor. If not, see <https://www.gnu.org/licenses/>.

/**
 * @file dynamic_types_utils.cpp
 */

#include <sstream>
#include <random>
#include <limits>

#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeMember.h>

#include "dynamic_types_utils.hpp"
#include "utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

std::vector<std::string> get_introspection_type_names(
        const TypeIntrospectionStruct& numeric_type_names)
{
    std::vector<std::string> type_names;
    for (auto& type_name : numeric_type_names)
    {
        type_names.push_back(std::get<0>(type_name));
    }
    return type_names;
}

void get_introspection_type_names(
        const std::string& base_type_name,
        eprosima::fastrtps::types::DynamicType_ptr type,
        TypeIntrospectionStruct& numeric_type_names,
        TypeIntrospectionStruct& string_type_names,
        const std::string& separator /* = "/" */)
{
    // TODO add return code checks

    // Using the Dynamic type, retrieve the name of the fields and its descriptions
    std::map<std::string, eprosima::fastrtps::types::DynamicTypeMember*> members_by_name;
    type->get_all_members_by_name(members_by_name);

    for (auto& members : members_by_name)
    {
        eprosima::fastrtps::types::TypeKind kind = members.second->get_descriptor()->get_kind();

        switch(kind)
        {
            case eprosima::fastrtps::types::TK_BOOLEAN:
            case eprosima::fastrtps::types::TK_BYTE:
            case eprosima::fastrtps::types::TK_INT16:
            case eprosima::fastrtps::types::TK_INT32:
            case eprosima::fastrtps::types::TK_INT64:
            case eprosima::fastrtps::types::TK_UINT16:
            case eprosima::fastrtps::types::TK_UINT32:
            case eprosima::fastrtps::types::TK_UINT64:
            case eprosima::fastrtps::types::TK_FLOAT32:
            case eprosima::fastrtps::types::TK_FLOAT64:
            case eprosima::fastrtps::types::TK_FLOAT128:
                // Numeric case
                numeric_type_names.push_back({base_type_name + separator + members.first, members.second->get_id(), kind});
                break;

            case eprosima::fastrtps::types::TK_CHAR8:
            case eprosima::fastrtps::types::TK_CHAR16:
            case eprosima::fastrtps::types::TK_STRING8:
            case eprosima::fastrtps::types::TK_STRING16:
            case eprosima::fastrtps::types::TK_ENUM:
                // String case
                string_type_names.push_back({base_type_name + separator + members.first, members.second->get_id(), kind});
                break;

            case eprosima::fastrtps::types::TK_NONE:
            case eprosima::fastrtps::types::TK_ALIAS:
            case eprosima::fastrtps::types::TK_BITMASK:
            case eprosima::fastrtps::types::TK_ANNOTATION:
            case eprosima::fastrtps::types::TK_STRUCTURE:
            case eprosima::fastrtps::types::TK_UNION:
            case eprosima::fastrtps::types::TK_BITSET:
            case eprosima::fastrtps::types::TK_SEQUENCE:
            case eprosima::fastrtps::types::TK_ARRAY:
            case eprosima::fastrtps::types::TK_MAP:
                // Complex types
                // TODO
                break;

            default:
                // Shouold not happen
                break;
        }
    }
}

void get_introspection_data(
        eprosima::fastrtps::types::DynamicType_ptr type,
        const TypeIntrospectionStruct& numeric_type_names,
        const TypeIntrospectionStruct& string_type_names,
        eprosima::fastrtps::types::DynamicData_ptr data,
        TypeIntrospectionNumericData& numeric_data,
        TypeIntrospectionStringData& string_data)
{
    // First get numeric data
    // It is used by index of the vector to avoid unnecessary lookups, but vectors must be sorted
    for (int i=0; i<numeric_type_names.size(); ++i)
    {
        numeric_data[i].second = get_numeric_type_from_data(
            data, std::get<1>(numeric_type_names[i]),
            std::get<2>(numeric_type_names[i]));
    }

    for (int i=0; i<string_type_names.size(); ++i)
    {
        string_data[i].second = get_string_type_from_data(
            data, std::get<1>(string_type_names[i]),
            std::get<2>(string_type_names[i]));
    }
}

double get_numeric_type_from_data(
        eprosima::fastrtps::types::DynamicData_ptr data,
        eprosima::fastrtps::types::MemberId member,
        eprosima::fastrtps::types::TypeKind kind)
{
    switch(kind)
    {
        case eprosima::fastrtps::types::TK_BOOLEAN:
            return static_cast<double>(data->get_bool_value(member));

        case eprosima::fastrtps::types::TK_BYTE:
            return static_cast<double>(data->get_byte_value(member));

        case eprosima::fastrtps::types::TK_INT16:
            return static_cast<double>(data->get_int16_value(member));

        case eprosima::fastrtps::types::TK_INT32:
            return static_cast<double>(data->get_int32_value(member));

        case eprosima::fastrtps::types::TK_INT64:
            return static_cast<double>(data->get_int64_value(member));

        case eprosima::fastrtps::types::TK_UINT16:
            return static_cast<double>(data->get_uint16_value(member));

        case eprosima::fastrtps::types::TK_UINT32:
            return static_cast<double>(data->get_uint32_value(member));

        case eprosima::fastrtps::types::TK_UINT64:
            return static_cast<double>(data->get_uint64_value(member));

        case eprosima::fastrtps::types::TK_FLOAT32:
            return static_cast<double>(data->get_float32_value(member));

        case eprosima::fastrtps::types::TK_FLOAT64:
            return static_cast<double>(data->get_float64_value(member));

        case eprosima::fastrtps::types::TK_FLOAT128:
            return static_cast<double>(data->get_float128_value(member));

        default:
            // The rest of values should not arrive here (are cut before when creating type names)
            break;
    }

    return std::numeric_limits<double>::min();
}

std::string get_string_type_from_data(
        eprosima::fastrtps::types::DynamicData_ptr data,
        eprosima::fastrtps::types::MemberId member,
        eprosima::fastrtps::types::TypeKind kind)
{
    switch(kind)
    {
        case eprosima::fastrtps::types::TK_CHAR8:
            return to_string(data->get_char8_value(member));

        case eprosima::fastrtps::types::TK_CHAR16:
            return to_string(data->get_char16_value(member));

        case eprosima::fastrtps::types::TK_STRING8:
            return data->get_string_value(member);

        case eprosima::fastrtps::types::TK_STRING16:
            return to_string(data->get_wstring_value(member));

        case eprosima::fastrtps::types::TK_ENUM:
            return data->get_enum_value(member);

        default:
            // The rest of values should not arrive here (are cut before when creating type names)
            break;
    }

    return "TYPE ERROR! Ask for help.";
}

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */
