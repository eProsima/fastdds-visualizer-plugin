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
 * @file Configuration.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_

#include <QStringList>
#include <QSettings>
#include <QDomDocument>

#include "utils/DataTypeConfiguration.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

/**
 * @brief This is a data structure to store the configuration of the plugin.
 */
struct Configuration
{

    Configuration();

    //! Load default configuration with specific prefix
    Configuration(
            const QString& prefix);

    Configuration(
            const Configuration& other) = default;
    Configuration& operator =(
            const Configuration& other) = default;

    ////////////////////
    // Topics
    QStringList topics_selected;  // Empty in initialization

    ////////////////////
    // Max array size
    DataTypeConfiguration data_type_configuration;

    ////////////////////
    // DDS Configuration
    unsigned int domain_id = 0;
    QStringList xml_datatypes_files;  // Empty in initialization
    // Discovery server configuration
    std::string server_ip = "127.0.0.1";
    unsigned int server_port = 11811;

    ////////////////////
    // Advance options
    bool boolean_strings_to_number = false;

    ////////////////////
    // Save Load configuration
    bool xmlSaveState(
            QDomDocument& doc,
            QDomElement& parent_element) const;
    bool xmlLoadState(
            const QDomElement& parent_element);

    void save_default_settings(
            const QString& prefix) const;

    void load_default_settings(
            const QString& prefix);

protected:

    constexpr static const char* MAX_ARRAY_SIZE_SETTINGS_TAG = "max_array_size";
    constexpr static const char* USE_HEADER_STAMP_SETTINGS_TAG = "use_header_stamp";
    constexpr static const char* DISCARD_LARGE_ARRAYS_SETTINGS_TAG = "discard_large_arrays";
    constexpr static const char* XML_DATATYPE_FILES_SETTINGS_TAG = "xml_datatype_files";
    constexpr static const char* DOMAIN_ID_SETTINGS_TAG = "domain_id_files";
    constexpr static const char* SERVER_IP_SETTINGS_TAG = "server_ip_files";
    constexpr static const char* SERVER_PORT_SETTINGS_TAG = "server_port_files";
};


} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_
