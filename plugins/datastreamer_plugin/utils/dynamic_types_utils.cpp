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

#include <sstream>
#include <random>
#include <limits>

#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicDataFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeMember.hpp>
#include <fastdds/dds/xtypes/dynamic_types/TypeDescriptor.hpp>

#include "dynamic_types_utils.hpp"
#include "utils.hpp"
#include "Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

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
        const fastdds::dds::DynamicType::_ref_type& type,
        const DataTypeConfiguration& data_type_configuration,
        TypeIntrospectionCollection& numeric_type_names,
        TypeIntrospectionCollection& string_type_names,
        DynamicData::_ref_type data /* = nullptr */,
        const std::vector<MemberId>& current_members_tree /* = {} */,
        const std::vector<TypeKind>& current_kinds_tree /* = {} */,
        const std::string& separator /* = "/" */)
{
    // Get type kind and store it as kind tree
    TypeKind kind = type->get_kind();

    DEBUG("Getting types for type member " << base_type_name << " of kind " + std::to_string(kind));

    switch (kind)
    {
        case TK_BOOLEAN:
        case TK_BYTE:
        case TK_INT8:
        case TK_INT16:
        case TK_INT32:
        case TK_INT64:
        case TK_UINT8:
        case TK_UINT16:
        case TK_UINT32:
        case TK_UINT64:
        case TK_FLOAT32:
        case TK_FLOAT64:
        case TK_FLOAT128:
            // Add numeric value
            numeric_type_names.push_back(
                {base_type_name, current_members_tree, current_kinds_tree, kind});
            break;

        case TK_CHAR8:
        case TK_CHAR16:
        case TK_STRING8:
        case TK_STRING16:
        case TK_ALIAS:
        case TK_ENUM:
            string_type_names.push_back(
                {base_type_name, current_members_tree, current_kinds_tree, kind});
            break;

        case TK_ARRAY:
        case TK_SEQUENCE:
        {
            // Get the DynamicType of the elements in the array/sequence
            DynamicType::_ref_type internal_type = type_internal_kind(type);
            std::string kind_str;
            unsigned int size;
            if (kind == TK_ARRAY)
            {
                kind_str = "array";
                size = array_size(type);
            }
            else if (kind == TK_SEQUENCE)
            {
                kind_str = "sequence";
                assert(data != nullptr);
                size = data->get_item_count();
            }

            // Allow this array depending on data type configuration
            if (size >= data_type_configuration.max_array_size)
            {
                if (data_type_configuration.discard_large_arrays)
                {
                    // Discard array/sequence
                    DEBUG("Discarding " << kind_str << " " << base_type_name << " of size " << size);
                    break;
                }
                else
                {
                    // Truncate array/sequence
                    DEBUG(
                        "Truncating " << kind_str << " " << base_type_name <<
                            " of size " << size <<
                            " to size " << data_type_configuration.max_array_size);
                    size = data_type_configuration.max_array_size;
                }
                // Could not be neither of them, it would be an inconsistency
            }

            for (MemberId member_id = 0; member_id < size; member_id++)
            {
                std::vector<MemberId> new_members_tree(current_members_tree);
                new_members_tree.push_back(member_id);
                std::vector<TypeKind> new_kinds_tree(current_kinds_tree);
                new_kinds_tree.push_back(kind);
                DynamicData::_ref_type member_data = data &&
                        is_type_complex(internal_type) ? data->loan_value(member_id) : nullptr;
                get_introspection_type_names(
                    base_type_name + "[" + std::to_string(member_id) + "]",
                    internal_type,
                    data_type_configuration,
                    numeric_type_names,
                    string_type_names,
                    member_data,
                    new_members_tree,
                    new_kinds_tree,
                    separator);

                if (member_data)
                {
                    data->return_loaned_value(member_data);
                }
            }
            break;
        }

        case TK_STRUCTURE:
        {
            // Using the Dynamic type, retrieve the name of the fields and its descriptions
            DynamicTypeMembersByName members_by_name;
            type->get_all_members_by_name(members_by_name);

            DEBUG("Size of members " << members_by_name.size());

            for (const auto& members : members_by_name)
            {
                DEBUG("Calling introspection for " << members.first);

                std::vector<MemberId> new_members_tree(current_members_tree);
                new_members_tree.push_back(members.second->get_id());
                std::vector<TypeKind> new_kinds_tree(current_kinds_tree);
                new_kinds_tree.push_back(kind);
                traits<eprosima::fastdds::dds::MemberDescriptor>::ref_type member_descriptor{traits<MemberDescriptor>::
                                                                                             make_shared()};

                if (RETCODE_OK != members.second->get_descriptor(member_descriptor))
                {
                    throw std::runtime_error("Error getting descriptor from DynamicTypeMember");
                }
                DynamicData::_ref_type member_data = data ? data->loan_value(members.second->get_id()) : nullptr;

                get_introspection_type_names(
                    base_type_name + separator + std::string(members.first),
                    member_descriptor->type(),
                    data_type_configuration,
                    numeric_type_names,
                    string_type_names,
                    member_data,
                    new_members_tree,
                    new_kinds_tree,
                    separator);

                if (member_data)
                {
                    data->return_loaned_value(member_data);
                }
            }
            break;
        }

        case TK_UNION:
        {

            // Check which member is active
            DynamicTypeMember::_ref_type discriminator;
            traits<eprosima::fastdds::dds::MemberDescriptor>::ref_type member_descriptor{traits<MemberDescriptor>::
                                                                                         make_shared()};

            if (RETCODE_OK != type->get_member_by_name(discriminator, "discriminator"))
            {
                throw std::runtime_error("Error getting discriminator member from union");
            }

            if (RETCODE_OK != discriminator->get_descriptor(member_descriptor))
            {
                throw std::runtime_error("Error getting descriptor from discriminator");
            }

            TypeKind discriminator_kind = member_descriptor->type()->get_kind();
            MemberId discriminator_id = member_descriptor->id();
            int32_t discriminator_value;
            DynamicTypeMembersByName members;
            MemberId active_member_id = MEMBER_ID_INVALID;

            if (RETCODE_OK != type->get_all_members_by_name(members))
            {
                throw std::runtime_error("Error getting members by name from DynamicType");
            }

            // If not data is provided, set active member to default
            if (!data)
            {
                DEBUG("No data provided, setting default member as active");
                
                for (const auto& member : members)
                {
                    if (member.second->get_descriptor(member_descriptor) != RETCODE_OK)
                    {
                        throw std::runtime_error("Error getting descriptor from DynamicTypeMember");
                    }

                    if (member_descriptor->is_default_label())
                    {
                        active_member_id = member.second->get_id();
                        break;
                    }
                }

            }
            else
            {
                for (const auto& member : members)
                {
                    DEBUG("Checking if member " << member.first << "is active");

                    // Get the member descriptor and member labels
                    if (RETCODE_OK != member.second->get_descriptor(member_descriptor))
                    {
                        throw std::runtime_error("Error getting descriptor from DynamicTypeMember");
                    }

                    const UnionCaseLabelSeq& member_label = member_descriptor->label();
                    if (RETCODE_OK != get_discriminator_value(discriminator_value, data, discriminator_id, discriminator_kind))
                    {
                        throw std::runtime_error("Error getting discriminator value from DynamicData");
                    }

                    if (std::find(member_label.begin(), member_label.end(), discriminator_value) != member_label.end())
                    {
                        active_member_id = member.second->get_id();
                        break;
                    }
                    else if (member_descriptor->is_default_label())
                    {
                        active_member_id = member.second->get_id();
                    }
                }
            }

            if (active_member_id == MEMBER_ID_INVALID)
            {
                throw std::runtime_error("Error getting active member in union");
            }

            std::vector<MemberId> new_members_tree(current_members_tree);
            new_members_tree.push_back(active_member_id);
            std::vector<TypeKind> new_kinds_tree(current_kinds_tree);
            new_kinds_tree.push_back(kind);

            DynamicData::_ref_type member_data = data ? data->loan_value(active_member_id) : nullptr;
            get_introspection_type_names(
                base_type_name + separator + std::string(member_descriptor->name()),
                member_descriptor->type(),
                data_type_configuration,
                numeric_type_names,
                string_type_names,
                member_data,
                new_members_tree,
                new_kinds_tree,
                separator);

            if (member_data)
            {
                data->return_loaned_value(member_data);
            }

            break;
        }
        case TK_MAP:
        {
            DynamicTypeMembersByName members_by_name;
            if (RETCODE_OK != type->get_all_members_by_name(members_by_name))
            {
                throw std::runtime_error("Error getting members by name from DynamicType");
            }
            for (const auto& member : members_by_name)
            {
                DEBUG("Calling introspection for " << member.first);
            }
        }
        // TODO: Add support for currently compatible types
        case TK_BITSET:
        case TK_NONE:
        default:
            WARNING(kind << " DataKind Not supported");
            throw std::runtime_error("Unsupported Dynamic Types kind");
    }
}

void get_introspection_numeric_data(
        const TypeIntrospectionCollection& numeric_type_names,
        DynamicData::_ref_type data,
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
        auto parent_data = get_parent_data_of_member(
            data,
            members,
            kinds);

        numeric_data_result[i].second = get_numeric_type_from_data(
            parent_data,
            members[members.size() - 1],             // use last member Id (direct parent = parent_data)
            actual_kind);                            // use last kind (direct parent = parent_data)
    }
}

void get_introspection_string_data(
        const TypeIntrospectionCollection& string_type_names,
        DynamicData::_ref_type data,
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
        auto parent_data = get_parent_data_of_member(
            data,
            members,
            kinds);

        string_data_result[i].second = get_string_type_from_data(
            parent_data,
            members[members.size() - 1],             // use last member Id (direct parent = parent_data)
            actual_kind);                            // use last kind (direct parent = parent_data)
    }
}

DynamicData::_ref_type get_parent_data_of_member(
        DynamicData::_ref_type data,
        const std::vector<MemberId>& members_tree,
        const std::vector<TypeKind>& kind_tree,
        unsigned int array_indexes /* = 0 */)
{

    DEBUG("Getting parent data of type " << std::to_string(kind_tree[array_indexes]));

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
            case TK_STRUCTURE:
            case TK_ARRAY:
            case TK_SEQUENCE:
            case TK_UNION:
            {
                // Access to the data inside the structure
                DynamicData::_ref_type child_data;
                // Get data pointer to the child_data
                // The loan and return is a workaround to avoid creating a unecessary copy of the data
                child_data = data->loan_value(member_id);
                if (RETCODE_OK != data->return_loaned_value(child_data))
                {
                    throw std::runtime_error("Error returning loaned value from DynamicData ");
                }

                return get_parent_data_of_member(
                    child_data,
                    members_tree,
                    kind_tree,
                    array_indexes + 1);
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
        DynamicData::_ref_type data,
        const MemberId& member,
        const TypeKind& kind)
{
    DEBUG("Getting numeric data of kind " << std::to_string(kind) << " in member " << member);

    switch (kind)
    {
        case TK_BOOLEAN:
            bool boolean_value;
            if (RETCODE_OK != data->get_boolean_value(boolean_value, member))
            {
                throw std::runtime_error("Error getting boolean value from DynamicData");
            }
            return static_cast<double>(boolean_value);

        case TK_BYTE:
            fastdds::rtps::octet byte_value;
            if (RETCODE_OK != data->get_byte_value(byte_value, member))
            {
                throw std::runtime_error("Error getting byte value from DynamicData");
            }
            return static_cast<double>(byte_value);

        case TK_INT8:
            int8_t int8_value;
            if (RETCODE_OK != data->get_int8_value(int8_value, member))
            {
                throw std::runtime_error("Error getting int8 value from DynamicData");
            }
            return static_cast<double>(int8_value);

        case TK_INT16:
            int16_t int16_value;
            if (RETCODE_OK != data->get_int16_value(int16_value, member))
            {
                throw std::runtime_error("Error getting int16 value from DynamicData");
            }
            return static_cast<double>(int16_value);

        case TK_INT32:
            int32_t int32_value;
            if (RETCODE_OK != data->get_int32_value(int32_value, member))
            {
                throw std::runtime_error("Error getting int32 value from DynamicData");
            }

            return static_cast<double>(int32_value);

        case TK_INT64:
            int64_t int64_value;
            if (RETCODE_OK != data->get_int64_value(int64_value, member))
            {

                throw std::runtime_error("Error getting int64 value from DynamicData");
            }

            return static_cast<double>(int64_value);

        case TK_UINT8:
            uint8_t uint8_value;
            if (RETCODE_OK != data->get_uint8_value(uint8_value, member))
            {

                throw std::runtime_error("Error getting int8 value from DynamicData");
            }

            return static_cast<double>(uint8_value);

        case TK_UINT16:
            uint16_t uint16_value;
            if (RETCODE_OK != data->get_uint16_value(uint16_value, member))

            {

                throw std::runtime_error("Error getting uint16 value from DynamicData");
            }

            return static_cast<double>(uint16_value);

        case TK_UINT32:
            uint32_t uint32_value;
            if (RETCODE_OK != data->get_uint32_value(uint32_value, member))
            {

                throw std::runtime_error("Error getting uint32 value from DynamicData");
            }

            return static_cast<double>(uint32_value);

        case TK_UINT64:
            uint64_t uint64_value;
            if (RETCODE_OK != data->get_uint64_value(uint64_value, member))
            {

                throw std::runtime_error("Error getting uint64 value from DynamicData");
            }

            return static_cast<double>(uint64_value);

        case TK_FLOAT32:
            float float32_value;
            if (RETCODE_OK != data->get_float32_value(float32_value, member))
            {

                throw std::runtime_error("Error getting float32 value from DynamicData");
            }
            return static_cast<double>(float32_value);

        case TK_FLOAT64:
            double float64_value;
            if (RETCODE_OK != data->get_float64_value(float64_value, member))
            {

                throw std::runtime_error("Error getting float64 value from DynamicData");
            }
            return float64_value;

        case TK_FLOAT128:
            long double float128_value;
            if (RETCODE_OK != data->get_float128_value(float128_value, member))
            {

                throw std::runtime_error("Error getting float128 value from DynamicData");
            }

            return static_cast<double>(float128_value);

        default:
            // The rest of values should not arrive here (are cut before when creating type names)
            throw InconsistencyException("Member wrongly set as numeric");
    }
}

std::string get_string_type_from_data(
        DynamicData::_ref_type data,
        const MemberId& member,
        const TypeKind& kind)
{
    DEBUG("Getting string data of kind " << std::to_string(kind) << " in member " << member);

    switch (kind)
    {
        case TK_CHAR8:
        {
            char char8_value;
            if (RETCODE_OK != data->get_char8_value(char8_value, member))
            {

                throw std::runtime_error("Error getting char8 value from DynamicData");
            }
            return to_string(char8_value);
        }
        case TK_CHAR16:
        {
            wchar_t char16_value;
            if (RETCODE_OK != data->get_char16_value(char16_value, member))
            {

                throw std::runtime_error("Error getting char16 value from DynamicData");
            }

            return to_string(char16_value);
        }
        case TK_STRING8:
        {
            std::string string8_value;
            if (RETCODE_OK != data->get_string_value(string8_value, member))
            {

                throw std::runtime_error("Error getting string8 value from DynamicData");
            }
            return string8_value;
        }
        case TK_STRING16:
        {
            std::wstring string16_value;
            if (RETCODE_OK != data->get_wstring_value(string16_value, member))
            {

                throw std::runtime_error("Error getting string16 value from DynamicData");
            }
            return to_string(string16_value);
        }
        case TK_ENUM:
        {
            // Obtain first the numeric value of the enum
            int32_t enum_value;
            if (RETCODE_OK != data->get_int32_value(enum_value, member))
            {

                throw std::runtime_error("Error getting enum value from DynamicData");
            }

            // Get enumeration type to obtain the names of the different values
            DynamicType::_ref_type enum_type;
            MemberDescriptor::_ref_type enum_desc{traits<MemberDescriptor>::make_shared()};
            if (RETCODE_OK != data->get_descriptor(enum_desc, member))
            {
                throw std::runtime_error("Error getting enum descriptor from DynamicData");
            }
            enum_type = enum_desc->type();

            DynamicTypeMembersByName enum_members;
            if (RETCODE_OK != enum_type->get_all_members_by_name(enum_members))
            {
                throw std::runtime_error("Error getting enum members from DynamicData");
            }
            // Get name associated to the numeric enum value previously obtained
            ObjectName member_name;
            for (const auto& enum_member : enum_members)
            {
                MemberDescriptor::_ref_type enum_member_desc{traits<MemberDescriptor>::make_shared()};
                if (RETCODE_OK != enum_member.second->get_descriptor(enum_member_desc))
                {
                    throw std::runtime_error("Error getting enum member descriptor from DynamicData");
                }
                // Check if it is the member we are looking for
                if (enum_member_desc->default_value() == std::to_string(enum_value))
                {
                    member_name = enum_member.first;
                    assert(member_name == enum_member.second->get_name());
                    break;
                }
            }

            return member_name.to_string();
        }
        default:
        {
            // The rest of values should not arrive here (are cut before when creating type names)
            throw InconsistencyException("Member wrongly set as string");
        }
    }
}

bool is_kind_numeric(
        const TypeKind& kind)
{
    switch (kind)
    {
        case TK_BOOLEAN:
        case TK_BYTE:
        case TK_INT8:
        case TK_INT16:
        case TK_INT32:
        case TK_INT64:
        case TK_UINT8:
        case TK_UINT16:
        case TK_UINT32:
        case TK_UINT64:
        case TK_FLOAT32:
        case TK_FLOAT64:
        case TK_FLOAT128:
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
        case TK_CHAR8:
        case TK_CHAR16:
        case TK_STRING8:
        case TK_STRING16:
        case TK_ENUM:
            return true;

        default:
            return false;
    }
}

DynamicType::_ref_type type_internal_kind(
        const DynamicType::_ref_type& dyn_type)
{
    TypeDescriptor::_ref_type type_descriptor{traits<TypeDescriptor>::make_shared()};

    if (RETCODE_OK != dyn_type->get_descriptor(type_descriptor))
    {
        throw std::runtime_error(std::string(
                          "Error getting descriptor from DynamicType ") + dyn_type->get_name().to_string());
    }

    return type_descriptor->element_type();
}

uint32_t array_size(
        const DynamicType::_ref_type& dyn_type)
{
    TypeDescriptor::_ref_type type_descriptor{traits<TypeDescriptor>::make_shared()};

    if (RETCODE_OK != dyn_type->get_descriptor(type_descriptor))
    {
        throw std::runtime_error(std::string(
                          "Error getting descriptor from DynamicType ") + dyn_type->get_name().to_string());
    }

    // Get array dimension
    BoundSeq array_bound = type_descriptor->bound();

    // Get array size
    if (array_bound.size() < 1)
    {
        throw std::runtime_error(std::string(
                          "Error getting array size from DynamicType ") + dyn_type->get_name().to_string());
    }
    uint32_t array_size = 1;
    for (auto bound : array_bound)
    {
        array_size *= bound;
    }
    return array_size;
}

// WARNING: Due to a discripency between xtypes 1.3 standard and DynamicTypes implementation, this function could lead to anomalous behavior
// when casting to int32 non-promotable types (e.g. int64) that are also possible discriminator types.
ReturnCode_t get_discriminator_value(
        int32_t& discriminator_value,
        DynamicData::_ref_type data,
        const MemberId& discriminator_id,
        const TypeKind& discriminator_kind)
{
    if (data)
    {
        ReturnCode_t retcode;

        switch (discriminator_kind)
        {
            case TK_BOOLEAN:
            {
                bool boolean_value;
                retcode = data->get_boolean_value(boolean_value, discriminator_id);

                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting boolean value from DynamicData");
                    return retcode;
                }

                discriminator_value = static_cast<int32_t>(boolean_value);
                return RETCODE_OK;
            }
            case TK_BYTE:
            {
                fastdds::rtps::octet byte_value;
                retcode = data->get_byte_value(byte_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting byte value from DynamicData");
                    return retcode;
                }

                discriminator_value = static_cast<int32_t>(byte_value);
                return RETCODE_OK;
            }
            case TK_CHAR8:
            {
                char char8_value;
                retcode = data->get_char8_value(char8_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting char8 value from DynamicData");
                    return retcode;
                }

                discriminator_value = static_cast<int32_t>(char8_value);
                return RETCODE_OK;
            }
            case TK_CHAR16:
            {
                wchar_t char16_value;
                retcode = data->get_char16_value(char16_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting char16 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(char16_value);
                return RETCODE_OK;
            }
            case TK_INT8:
            {
                int8_t int8_value;
                retcode = data->get_int8_value(int8_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting int8 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(int8_value);
                return RETCODE_OK;
            }
            case TK_UINT8:
            {
                uint8_t uint8_value;
                retcode = data->get_uint8_value(uint8_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting uint8 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(uint8_value);
                return RETCODE_OK;
            }
            case TK_INT16:
            {
                int16_t int16_value;
                retcode = data->get_int16_value(int16_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting int16 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(int16_value);
                return RETCODE_OK;
            }
            case TK_UINT16:
            {
                uint16_t uint16_value;
                retcode = data->get_uint16_value(uint16_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting uint16 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(uint16_value);
                return RETCODE_OK;
            }
            case TK_INT32:
            {
                int32_t int32_value;
                retcode = data->get_int32_value(int32_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting int32 value from DynamicData");
                    return retcode;
                }
                discriminator_value = int32_value;
                return RETCODE_OK;
            }
            case TK_UINT32:
            {
                uint32_t uint32_value;
                retcode = data->get_uint32_value(uint32_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting uint32 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(uint32_value);
                return RETCODE_OK;
            }
            case TK_INT64:
            {
                int64_t int64_value;
                retcode = data->get_int64_value(int64_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting int64 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<uint32_t>(int64_value);
                return RETCODE_OK;
            }
            case TK_UINT64:
            {
                uint64_t uint64_value;
                retcode = data->get_uint64_value(uint64_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting uint64 value from DynamicData");
                    return retcode;
                }
                discriminator_value = static_cast<int32_t>(uint64_value);
                return RETCODE_OK;
            }
            case TK_ENUM:
            {
                int32_t enum_value;
                retcode = data->get_int32_value(enum_value, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting enum value from DynamicData");
                    return retcode;
                }
                discriminator_value = enum_value;
                return RETCODE_OK;
            }
            case TK_ALIAS:
            {
                traits<MemberDescriptor>::ref_type member_descriptor{traits<MemberDescriptor>::make_shared()};
                retcode = data->get_descriptor(member_descriptor, discriminator_id);
                if (RETCODE_OK != retcode)
                {
                    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting descriptor from DynamicData");
                    return retcode;
                }
                get_discriminator_value(discriminator_value, data, discriminator_id,
                        member_descriptor->type()->get_kind());
                break;
            }
            default:
                EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Unsupported discriminator type");
                return RETCODE_BAD_PARAMETER;
        }
    }

    EPROSIMA_LOG_ERROR(DYNAMIC_TYPES_UTILS, "Error getting discriminator value from DynamicData: data is nullptr");
    return RETCODE_NO_DATA;
    
}

bool is_type_static(
        const DynamicType::_ref_type& dyn_type)
{
    // Get type kind and store it as kind tree
    TypeKind kind = dyn_type->get_kind();

    switch (kind)
    {
        case TK_BOOLEAN:
        case TK_BYTE:
        case TK_INT16:
        case TK_INT32:
        case TK_INT64:
        case TK_UINT16:
        case TK_UINT32:
        case TK_UINT64:
        case TK_FLOAT32:
        case TK_FLOAT64:
        case TK_FLOAT128:
        case TK_CHAR8:
        case TK_CHAR16:
        case TK_STRING8:
        case TK_STRING16:
        case TK_ENUM:
        case TK_BITSET:
        case TK_BITMASK:
        case TK_ARRAY:
            return true;

        case TK_SEQUENCE:
        case TK_MAP:
        case TK_UNION:
            return false;

        case TK_STRUCTURE:
        {
            // Using the Dynamic type, retrieve the name of the fields and its descriptions
            DynamicTypeMembersByName members_by_name;
            dyn_type->get_all_members_by_name(members_by_name);
            MemberDescriptor::_ref_type member_descriptor{traits<MemberDescriptor>::make_shared()};
            bool ret = true;
            for (const auto& members : members_by_name)
            {
                if (RETCODE_OK != members.second->get_descriptor(member_descriptor))
                {
                    throw std::runtime_error("Error getting descriptor from DynamicTypeMember");
                }
                ret = ret & is_type_static(member_descriptor->type());
            }
            return ret;
        }

        case TK_NONE:
        default:
            WARNING(kind << " DataKind Not supported");
            throw std::runtime_error("Unsupported Dynamic Types kind");
    }
}

bool is_type_complex(
        const DynamicType::_ref_type& dyn_type)
{
    TypeKind kind = dyn_type->get_kind();
    switch (kind)
    {
        case TK_ANNOTATION:
        case TK_ARRAY:
        case TK_BITMASK:
        case TK_BITSET:
        case TK_MAP:
        case TK_SEQUENCE:
        case TK_STRUCTURE:
        case TK_UNION:
            return true;
        default:
            return false;
    }
}

}         /* namespace utils */
}     /* namespace plotjuggler */
} /* namespace eprosima */
