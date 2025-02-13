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

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_LISTENER_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_LISTENER_HPP_

#include <QObject>

#include "utils/utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

/**
 * @brief TODO
 */
struct UiListener
{
public:

    virtual void on_xml_datatype_file_added(
            const std::string& file_path)
    {
        DEBUG("Calling on_xml_datatype_file_added " << file_path);
        static_cast<void>(file_path);
    }

    virtual void on_domain_connection(
            unsigned int domain_id)
    {
        DEBUG("Calling on_domain_connection " << domain_id);
        static_cast<void>(domain_id);
    }

    virtual void on_server_connection(
        unsigned int domain_id, const std::string& server_ip, unsigned int server_port){
        DEBUG("Calling on_server_connection " << domain_id << " " << server_ip << " " << server_port);
        static_cast<void>(domain_id);
        static_cast<void>(server_ip);
        static_cast<void>(server_port);
    }
};

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_LISTENER_HPP_
