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
 * @file TopicDataBase.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_TOPICDATABASE_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_TOPICDATABASE_HPP_

#include <unordered_map>
#include <tuple>

#include <fastdds/dds/xtypes/type_representation/detail/dds_xtypes_typeobject.hpp>

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

/**
 * @brief TODO
 *
 */
using TopicName = std::string;
using DataTypeNameType = std::string;
using DataTypeId = eprosima::fastdds::dds::xtypes::TypeIdentifier;
using TypeInfoAvailable = bool;
using DataTypeRegistryInfo = std::pair<DataTypeNameType, TypeInfoAvailable>;
using DataTypeIdInfo = std::pair<DataTypeNameType, DataTypeId>;
using TopicDataBase = std::unordered_map<TopicName, DataTypeRegistryInfo>;
using TopicIds = std::unordered_map<TopicName, DataTypeIdInfo>;

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_TOPICDATABASE_HPP_
