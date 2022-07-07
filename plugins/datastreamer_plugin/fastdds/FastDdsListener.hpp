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
 * @file UiListener.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_LISTENER_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_LISTENER_HPP_

#include <string>

#include "utils/utils.hpp"
#include "utils/types.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

/**
 * @brief This class transmit callbacks that arrive from Fast DDS entities to the GUI.
 *
 * Every new discovery and every new data received from Fast DDS entities is transmitted by calling
 * one of the methods, that must be implemented by the user of the class.
 */
class FastDdsListener
{
public:

    virtual void on_double_data_read(
            const std::vector<types::NumericDatum>& numeric_data,
            double timestamp)
    {
        DEBUG("Calling on_double_data_read");
        static_cast<void>(numeric_data);
        static_cast<void>(timestamp);
    }

    virtual void on_string_data_read(
            const std::vector<types::TextDatum>& text_data,
            double timestamp)
    {
        DEBUG("Calling on_string_data_read");
        static_cast<void>(text_data);
        static_cast<void>(timestamp);
    }

    virtual void on_topic_discovery(
            const std::string& topic_name,
            const std::string& type_name,
            bool type_registered)
    {
        DEBUG("Calling on_topic_discovery");
        static_cast<void>(topic_name);
        static_cast<void>(type_name);
        static_cast<void>(type_registered);
    }

};

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_LISTENER_HPP_
