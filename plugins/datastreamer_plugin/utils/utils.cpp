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
 * @file utils.cpp
 */

#include <sstream>

#include "utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace utils {

std::string get_timestamp_string(
        const fastrtps::rtps::Time_t& timestamp)
{
    std::stringstream ss;
    ss << timestamp;
    return ss.str();
}

double get_timestamp_seconds_numeric_value(
        const fastrtps::rtps::Time_t& timestamp)
{
    return (timestamp.seconds() + (timestamp.nanosec() * 1e-9));
}

std::string QString_to_string(
        const QString& str)
{
    return str.toStdString();
}

QString string_to_QString(
        const std::string& str)
{
    // return QString::fromStdString(str);
    return QString::fromUtf8( str.data(), str.size());
}

std::string to_string(
        const std::wstring& str)
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

std::string to_string(
        char c)
{
    return std::string(1, c);
}

std::string to_string(
        wchar_t c)
{
    return to_string(std::wstring(1, c));
}

} /* namespace utils */
} /* namespace plotjuggler */
} /* namespace eprosima */
