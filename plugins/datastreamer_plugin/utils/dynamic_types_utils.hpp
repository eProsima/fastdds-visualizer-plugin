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
#include "utils/types.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

using SingleDataTypeIntrospectionInfo =
    std::tuple<
        types::DatumLabel,
        eprosima::fastrtps::types::MemberId,
        eprosima::fastrtps::types::TypeKind>;
using TypeIntrospectionCollection =
        std::vector<SingleDataTypeIntrospectionInfo>;

using TypeIntrospectionNumericDatum = std::vector<types::NumericDatum>;
using TypeIntrospectionStringDatum = std::vector<types::TextDatum>;

std::vector<std::string> get_introspection_type_names(
        const TypeIntrospectionCollection& numeric_type_names);

void get_introspection_type_names(
        const std::string& base_type_name,
        eprosima::fastrtps::types::DynamicType_ptr type,
        TypeIntrospectionCollection& numeric_type_names,
        TypeIntrospectionCollection& string_type_names,
        const std::string& separator = "/");

void get_introspection_data(
        eprosima::fastrtps::types::DynamicType_ptr type,
        const TypeIntrospectionCollection& numeric_type_names,
        const TypeIntrospectionCollection& string_type_names,
        eprosima::fastrtps::types::DynamicData_ptr data,
        TypeIntrospectionNumericDatum& numeric_data,
        TypeIntrospectionStringDatum& string_data);

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
