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

#include "dynamic_types_utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

std::vector<std::pair<std::string, double>> get_numeric_data(
        const std::string& member_name,
        const eprosima::fastrtps::types::DynamicData_ptr& data)
{
    // TODO

    // Dummy answer
    std::vector<std::pair<std::string, double>> result;
    result.push_back(std::make_pair(member_name, static_cast<double>(std::rand() % 10)));

    return result;
}

std::vector<std::pair<std::string, std::string>> get_string_data(
        const std::string& member_name,
        const eprosima::fastrtps::types::DynamicData_ptr& data)
{
    // TODO

    // Dummy answer
    std::vector<std::pair<std::string, std::string>> result;
    result.push_back(std::make_pair(member_name, "ToDo"));

    return result;
}

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */
