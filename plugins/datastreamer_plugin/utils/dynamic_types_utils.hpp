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

#include <fastrtps/types/DynamicData.h>
#include "utils/types.hpp"
#include "utils/DataTypeConfiguration.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

using namespace eprosima::fastrtps::types;

using SingleDataTypeIntrospectionInfo =
        std::tuple<
    types::DatumLabel,
    std::vector<MemberId>,           // Member Ids of the parents of the data member
    std::vector<TypeKind>,           // Kind of the parents of the data member
    TypeKind>;                       // Kind of this member

using TypeIntrospectionCollection =
        std::vector<SingleDataTypeIntrospectionInfo>;

using TypeIntrospectionNumericStruct = std::vector<types::NumericDatum>;
using TypeIntrospectionStringStruct = std::vector<types::TextDatum>;

/**
 * @brief Get the names of each label in a \c TypeIntrospectionCollection
 * This function is used to obtain all the labels form by topics + data type members
 *
 * Iterate over every \c SingleDataTypeIntrospectionInfo and get the \c DatumLabel .
 *
 * @param numeric_type_names collection to get all names
 * @return all label names
 */
std::vector<std::string> get_introspection_type_names(
        const TypeIntrospectionCollection& numeric_type_names);

/**
 * @brief Get the introspection information from a \c DynamicType_ptr
 * This is a recursive function that may be called recursively for each data type member
 * of \c type .
 *
 * If \c type is a basic type, it adds its label, that is \c base_type_name to the
 * \c TypeIntrospectionCollection that it belongs, depending if it is numeric or text kind.
 * Along with the label, it is added the vector of parents as member Ids and kinds.
 * Those are useful to allow to acces this specific field from a \c DynamicData* .
 *
 * If \c type is not a basic type (array or struct) this function is called for each value
 * updating \c base_type_name for each recursive call.
 *
 * \c current_members_tree and \c current_kinds_tree are used to store the parent information
 * for every recursive iteration of the function.
 *
 * @param base_type_name [in]
 * @param type [in]
 * @param numeric_type_names [out]
 * @param string_type_names [out]
 * @param current_members_tree [in]
 * @param current_kinds_tree [in]
 * @param separator [in]
 */
void get_introspection_type_names(
        const std::string& base_type_name,
        const DynamicType_ptr& type,
        const DataTypeConfiguration& data_type_configuration,
        TypeIntrospectionCollection& numeric_type_names,
        TypeIntrospectionCollection& string_type_names,
        const std::vector<MemberId>& current_members_tree = {},
        const std::vector<TypeKind>& current_kinds_tree = {},
        const std::string& separator = "/");

/**
 * @brief Fill numeric and string data from a \c data given
 *
 * This method uses the already calculated \c SingleDataTypeIntrospectionInfo objects
 * to make data introspection faster, by access directly the members required (to access those members
 * it is needed the kind of each, that is why it is stored).
 *
 * The data found along the data is stored in object \c numeric_data .
 * This object is already created and populated with the labels, and it is accessed by
 * the same index in \c numeric_type_names .
 * This way, we avoid creating and copying the strings each time a data arrive.
 * Instead, we reuse the whole structure with labels, and with each data is populated with the new data recieved.
 *
 * @param numeric_type_names [in]
 * @param data [in]
 * @param numeric_data [in|out]
 */
void get_introspection_numeric_data(
        const TypeIntrospectionCollection& numeric_type_names,
        DynamicData* data,
        TypeIntrospectionNumericStruct& numeric_data_result);

/**
 * @brief TODO
 */
void get_introspection_string_data(
        const TypeIntrospectionCollection& string_type_names,
        DynamicData* data,
        TypeIntrospectionStringStruct& string_data_result);

/**
 * @brief Get the parent data from a \c data given following the chaing of
 * parents given their parent \c members and \c kinds .
 *
 * This recursive function calls recursively to itself with the next data member of
 * \c data following the member and kind indicated by \c array_index , that changes in every iteration
 *
 * @param data [in]
 * @param members_tree [in]
 * @param kind [in]
 * @param array_indexes [in]
 * @return DynamicData*
 */
DynamicData* get_parent_data_of_member(
        DynamicData* data,
        const std::vector<MemberId>& members,
        const std::vector<TypeKind>& kinds,
        unsigned int array_index = 0);

/**
 * @brief Get the numeric value of the data member of \c data given and with \c member id and \c kind given
 *
 * @param data [in]
 * @param member [in]
 * @param kind [in]
 * @return numeric data of the member casted to double
 */
double get_numeric_type_from_data(
        DynamicData* data,
        const MemberId& member,
        const TypeKind& kind);

/**
 * @brief Get the string value of the data member of \c data given and with \c member id and \c kind given
 *
 * @param data [in]
 * @param member [in]
 * @param kind [in]
 * @return string data of the member casted to string
 */
std::string get_string_type_from_data(
        DynamicData* data,
        const MemberId& member,
        const TypeKind& kind);

bool is_kind_numeric(
        const TypeKind& kind);

bool is_kind_string(
        const TypeKind& kind);

DynamicType_ptr array_internal_kind(
        const DynamicType_ptr& dyn_type);

unsigned int array_size(
        const DynamicType_ptr& dyn_type);

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_DYNAMICTYPESUTILS_HPP_
