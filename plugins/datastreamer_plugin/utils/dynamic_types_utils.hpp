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
 * @file dynamic_types_utils.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_

#include <fastdds/dds/xtypes/dynamic_types/DynamicData.hpp>

#include "utils/DataTypeConfiguration.hpp"
#include "utils/types.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

using TypeIntrospectionNumericStruct = std::vector<types::NumericDatum>;
using TypeIntrospectionStringStruct = std::vector<types::TextDatum>;


/**
 * @brief This function is used to obtain all the names of a given collection of numeric/string data.
 *
 * Iterate over every member and get name.
 *
 * @param numeric_type_names collection to get all names
 * @return all label names
 */
template <typename T>
std::vector<std::string> get_introspection_type_names(
        const T& type_names_struct);

void get_formatted_data(
        const std::string& base_type_name,
        const DataTypeConfiguration& data_type_configuration,
        TypeIntrospectionNumericStruct& numeric_data,
        TypeIntrospectionStringStruct& string_data,
        const nlohmann::json& data,
        const std::string& separator = "/");

bool is_kind_numeric(
        const nlohmann::json& data);

bool is_kind_boolean(
        const nlohmann::json& data);

bool is_kind_string(
        const nlohmann::json& data);

eprosima::fastdds::dds::ReturnCode_t serialize_data (
        eprosima::fastdds::dds::DynamicData::_ref_type data,
        nlohmann::json& serialized_data);

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_
