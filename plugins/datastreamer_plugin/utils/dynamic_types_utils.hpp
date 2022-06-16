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
 * @file dynamic_types_utils.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_

#include <fastrtps/types/DynamicData.h>

namespace eprosima {
namespace plotjuggler {
namespace utils {

using TypeIntrospectionStruct =
    std::vector<std::tuple<std::string, eprosima::fastrtps::types::MemberId, eprosima::fastrtps::types::TypeKind>>;
using TypeIntrospectionNumericData = std::vector<std::pair<std::string, double>>;
using TypeIntrospectionStringData = std::vector<std::pair<std::string, std::string>>;

std::vector<std::string> get_introspection_type_names(
        const TypeIntrospectionStruct& numeric_type_names);

void get_introspection_type_names(
        const std::string& base_type_name,
        eprosima::fastrtps::types::DynamicType_ptr type,
        TypeIntrospectionStruct& numeric_type_names,
        TypeIntrospectionStruct& string_type_names,
        const std::string& separator = "/");

void get_introspection_data(
        eprosima::fastrtps::types::DynamicType_ptr type,
        const TypeIntrospectionStruct& numeric_type_names,
        const TypeIntrospectionStruct& string_type_names,
        eprosima::fastrtps::types::DynamicData_ptr data,
        TypeIntrospectionNumericData& numeric_data,
        TypeIntrospectionStringData& string_data);

double get_numeric_type_from_data(
        eprosima::fastrtps::types::DynamicData_ptr data,
        eprosima::fastrtps::types::MemberId member,
        eprosima::fastrtps::types::TypeKind kind);

std::string get_string_type_from_data(
        eprosima::fastrtps::types::DynamicData_ptr data,
        eprosima::fastrtps::types::MemberId member,
        eprosima::fastrtps::types::TypeKind kind);

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_
