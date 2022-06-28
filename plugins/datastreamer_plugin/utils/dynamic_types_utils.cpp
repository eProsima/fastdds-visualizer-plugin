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
#include <fastrtps/types/TypeDescriptor.h>

#include "dynamic_types_utils.hpp"
#include "utils.hpp"
#include "Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

using namespace eprosima::fastrtps::types;

std::vector<std::string> get_introspection_type_names(
        const TypeIntrospectionCollection& numeric_type_names)
{
    std::vector<std::string> type_names;
    for (const auto& type_name : numeric_type_names)
    {
        type_names.push_back(std::get<types::DatumLabel>(type_name));
    }
    return type_names;
}

void get_introspection_type_names(
        const std::string& base_type_name,
        const DynamicType_ptr& type,
        TypeIntrospectionCollection& numeric_type_names,
        TypeIntrospectionCollection& string_type_names,
        const std::vector<MemberId>& current_members_tree /* = {} */,
        const std::vector<TypeKind>& current_kinds_tree /* = {} */,
        const std::string& separator /* = "/" */)
{
    DEBUG("Getting types for type member " << base_type_name);

    // Get type kind and store it as kind tree
    TypeKind kind = type->get_kind();

    switch (kind)
    {
        case fastrtps::types::TK_BOOLEAN:
        case fastrtps::types::TK_BYTE:
        case fastrtps::types::TK_INT16:
        case fastrtps::types::TK_INT32:
        case fastrtps::types::TK_INT64:
        case fastrtps::types::TK_UINT16:
        case fastrtps::types::TK_UINT32:
        case fastrtps::types::TK_UINT64:
        case fastrtps::types::TK_FLOAT32:
        case fastrtps::types::TK_FLOAT64:
        case fastrtps::types::TK_FLOAT128:
            // Add numeric value
            numeric_type_names.push_back(
                {base_type_name, current_members_tree, current_kinds_tree, kind});
            break;

        case fastrtps::types::TK_CHAR8:
        case fastrtps::types::TK_CHAR16:
        case fastrtps::types::TK_STRING8:
        case fastrtps::types::TK_STRING16:
        case fastrtps::types::TK_ENUM:
            string_type_names.push_back(
                {base_type_name, current_members_tree, current_kinds_tree, kind});
            break;

        case fastrtps::types::TK_ARRAY:
        {
            DynamicType_ptr internal_type =
                    array_internal_kind(type);
            unsigned int this_array_size = array_size(type);

            for (unsigned int i = 0; i < this_array_size; i++)
            {
                std::vector<MemberId> new_members_tree(current_members_tree);
                new_members_tree.push_back(i);
                std::vector<TypeKind> new_kinds_tree(current_kinds_tree);
                new_kinds_tree.push_back(kind);

                get_introspection_type_names(
                    base_type_name + separator + std::to_string(i),
                    internal_type,
                    numeric_type_names,
                    string_type_names,
                    new_members_tree,
                    new_kinds_tree,
                    separator);
            }
            break;
        }

        case fastrtps::types::TK_STRUCTURE:
        {
            // Using the Dynamic type, retrieve the name of the fields and its descriptions
            std::map<std::string, DynamicTypeMember*> members_by_name;
            type->get_all_members_by_name(members_by_name);

            DEBUG("Size of members " << members_by_name.size());

            for (const auto& members : members_by_name)
            {
                DEBUG("Calling introspection for " << members.first);

                std::vector<MemberId> new_members_tree(current_members_tree);
                new_members_tree.push_back(members.second->get_id());
                std::vector<TypeKind> new_kinds_tree(current_kinds_tree);
                new_kinds_tree.push_back(kind);

                get_introspection_type_names(
                    base_type_name + separator + members.first,
                    members.second->get_descriptor()->get_type(),
                    numeric_type_names,
                    string_type_names,
                    new_members_tree,
                    new_kinds_tree,
                    separator);
            }
            break;
        }

        case fastrtps::types::TK_BITSET:
        case fastrtps::types::TK_UNION:
        case fastrtps::types::TK_SEQUENCE:
        case fastrtps::types::TK_MAP:
        case fastrtps::types::TK_NONE:
        default:
            WARNING(kind << " DataKind Not supported");
            throw std::runtime_error("Unsupported Dynamic Types kind");
    }
}

void get_introspection_numeric_data(
        const TypeIntrospectionCollection& numeric_type_names,
        const DynamicData_ptr& data,
        TypeIntrospectionNumericStruct& numeric_data_result)
{
    DEBUG("Getting numeric data");

    if (numeric_type_names.size() != numeric_data_result.size())
    {
        throw InconsistencyException("Vector of struct and result sizes mismatch in get_introspection_numeric_data");
    }

    // It is used by index of the vector to avoid unnecessary lookups.
    // Vectors must be sorted in the same order => creation order given in get_introspection_type_names
    for (int i = 0; i < numeric_type_names.size(); ++i)
    {
        const auto& member_type_info = numeric_type_names[i];

        // Get reference to variables to avoid calling get twice
        const auto& members =
                std::get<std::vector<MemberId>>(member_type_info);
        const auto& kinds =
                std::get<std::vector<TypeKind>>(member_type_info);

        const TypeKind& actual_kind =
                std::get<TypeKind>(member_type_info);

        // Get Data parent that has the member we are looking for
        const auto& parent_data = get_parent_data_of_member(
            data,
            members,
            kinds);

        numeric_data_result[i].second = get_numeric_type_from_data(
            parent_data,
            members[members.size() - 1], // use last member Id (direct parent = parent_data)
            actual_kind);    // use last kind (direct parent = parent_data)
    }
}

void get_introspection_string_data(
        const TypeIntrospectionCollection& string_type_names,
        const eprosima::fastrtps::types::DynamicData_ptr& data,
        TypeIntrospectionStringStruct& string_data_result)
{
    DEBUG("Getting string data");

    if (string_type_names.size() != string_data_result.size())
    {
        throw InconsistencyException("Vector of struct and result sizes mismatch in get_introspection_string_data");
    }

    // It is used by index of the vector to avoid unnecessary lookups.
    // Vectors must be sorted in the same order => creation order given in get_introspection_type_names
    for (int i = 0; i < string_type_names.size(); ++i)
    {
        const auto& member_type_info = string_type_names[i];

        // Get reference to variables to avoid calling get twice
        const auto& members =
                std::get<std::vector<MemberId>>(member_type_info);
        const auto& kinds =
                std::get<std::vector<TypeKind>>(member_type_info);

        const TypeKind& actual_kind =
                std::get<TypeKind>(member_type_info);

        // Get Data parent that has the member we are looking for
        const auto& parent_data = get_parent_data_of_member(
            data,
            members,
            kinds);

        string_data_result[i].second = get_string_type_from_data(
            parent_data,
            members[members.size() - 1], // use last member Id (direct parent = parent_data)
            actual_kind);    // use last kind (direct parent = parent_data)
    }
}

DynamicData_ptr get_parent_data_of_member(
        const DynamicData_ptr& data,
        const std::vector<MemberId>& members_tree,
        const std::vector<TypeKind>& kind_tree,
        unsigned int array_indexes /* = 0 */)
{

    if (array_indexes == members_tree.size() - 1)
    {
        // One-to-last value, so this one is the value to take, as it is the parent of the data
        return data;
    }
    else
    {
        // Get the member and kind of the current level
        const MemberId& member_id = members_tree[array_indexes];
        const TypeKind& kind = kind_tree[array_indexes];

        switch (kind)
        {
            case fastrtps::types::TK_STRUCTURE:
            {
                // Access to the data inside the structure
                DynamicData* child_data;
                data->get_complex_value(&child_data, member_id);

                return get_parent_data_of_member(
                    DynamicData_ptr(child_data),
                    members_tree,
                    kind_tree,
                    array_indexes + 1);
            }
            case fastrtps::types::TK_ARRAY:
            {
                // TODO (this is not so important as a type with a complex array will die before
                // arriving here)
            }
            // TODO
            default:
                // TODO add exception
                WARNING("Error getting data");
                return data;
                break;
        }
    }
}

double get_numeric_type_from_data(
        const DynamicData_ptr& data,
        const MemberId& member,
        const TypeKind& kind)
{
    DEBUG("Getting numeric data of kind " << kind << " in member " << member);

    switch (kind)
    {
        case fastrtps::types::TK_BOOLEAN:
            return static_cast<double>(data->get_bool_value(member));

        case fastrtps::types::TK_BYTE:
            return static_cast<double>(data->get_byte_value(member));

        case fastrtps::types::TK_INT16:
            return static_cast<double>(data->get_int16_value(member));

        case fastrtps::types::TK_INT32:
            return static_cast<double>(data->get_int32_value(member));

        case fastrtps::types::TK_INT64:
            return static_cast<double>(data->get_int64_value(member));

        case fastrtps::types::TK_UINT16:
            return static_cast<double>(data->get_uint16_value(member));

        case fastrtps::types::TK_UINT32:
            return static_cast<double>(data->get_uint32_value(member));

        case fastrtps::types::TK_UINT64:
            return static_cast<double>(data->get_uint64_value(member));

        case fastrtps::types::TK_FLOAT32:
            return static_cast<double>(data->get_float32_value(member));

        case fastrtps::types::TK_FLOAT64:
            return static_cast<double>(data->get_float64_value(member));

        case fastrtps::types::TK_FLOAT128:
            return static_cast<double>(data->get_float128_value(member));

        default:
            // The rest of values should not arrive here (are cut before when creating type names)
            throw InconsistencyException("Member wrongly set as numeric");
    }
}

std::string get_string_type_from_data(
        const DynamicData_ptr& data,
        const MemberId& member,
        const TypeKind& kind)
{
    DEBUG("Getting string data of kind " << kind << " in member " << member);
    switch (kind)
    {
        case fastrtps::types::TK_CHAR8:
            return to_string(data->get_char8_value(member));

        case fastrtps::types::TK_CHAR16:
            return to_string(data->get_char16_value(member));

        case fastrtps::types::TK_STRING8:
            return data->get_string_value(member);

        case fastrtps::types::TK_STRING16:
            return to_string(data->get_wstring_value(member));

        case fastrtps::types::TK_ENUM:
            return data->get_enum_value(member);

        default:
            // The rest of values should not arrive here (are cut before when creating type names)
            throw InconsistencyException("Member wrongly set as string");
    }
}

bool is_kind_numeric(
        const TypeKind& kind)
{
    switch (kind)
    {
        case fastrtps::types::TK_BOOLEAN:
        case fastrtps::types::TK_BYTE:
        case fastrtps::types::TK_INT16:
        case fastrtps::types::TK_INT32:
        case fastrtps::types::TK_INT64:
        case fastrtps::types::TK_UINT16:
        case fastrtps::types::TK_UINT32:
        case fastrtps::types::TK_UINT64:
        case fastrtps::types::TK_FLOAT32:
        case fastrtps::types::TK_FLOAT64:
        case fastrtps::types::TK_FLOAT128:
            return true;

        default:
            return false;
    }
}

bool is_kind_string(
        const TypeKind& kind)
{
    switch (kind)
    {
        case fastrtps::types::TK_CHAR8:
        case fastrtps::types::TK_CHAR16:
        case fastrtps::types::TK_STRING8:
        case fastrtps::types::TK_STRING16:
        case fastrtps::types::TK_ENUM:
            return true;

        default:
            return false;
    }
}

DynamicType_ptr array_internal_kind(
        const DynamicType_ptr& dyn_type)
{
    return dyn_type->get_descriptor()->get_element_type();
}

unsigned int array_size(
        const DynamicType_ptr& dyn_type)
{
    return dyn_type->get_descriptor()->get_total_bounds();
}

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */
