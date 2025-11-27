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
 * @file Configuration.cpp
 */

#include "Configuration.hpp"
#include "utils/utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

Configuration::Configuration()
{
}

Configuration::Configuration(
        const QString& prefix)
{
    load_default_settings(prefix);
}

////////////////////
// Save Load configuration
bool Configuration::xmlSaveState(
        QDomDocument& doc,
        QDomElement& parent_element) const
{
    DEBUG("Saving XML configuration state");

    QDomElement max_elem = doc.createElement(MAX_ARRAY_SIZE_SETTINGS_TAG);
    max_elem.setAttribute("value", QString::number(data_type_configuration.max_array_size));
    parent_element.appendChild(max_elem);

    QDomElement stamp_elem = doc.createElement(USE_HEADER_STAMP_SETTINGS_TAG);
    stamp_elem.setAttribute("value", data_type_configuration.use_header_stamp ? "true" : "false");
    parent_element.appendChild(stamp_elem);

    QDomElement discard_elem = doc.createElement(DISCARD_LARGE_ARRAYS_SETTINGS_TAG);
    discard_elem.setAttribute("value", data_type_configuration.discard_large_arrays ? "true" : "false");
    parent_element.appendChild(discard_elem);

    return true;
}

bool Configuration::xmlLoadState(
        const QDomElement& parent_element)
{
    DEBUG("Loading XML configuration state");

    QDomElement max_elem = parent_element.firstChildElement(MAX_ARRAY_SIZE_SETTINGS_TAG);
    data_type_configuration.max_array_size = max_elem.attribute("value").toInt();

    QDomElement stamp_elem = parent_element.firstChildElement(USE_HEADER_STAMP_SETTINGS_TAG);
    data_type_configuration.use_header_stamp = (stamp_elem.attribute("value") == "true");

    QDomElement discard_elem = parent_element.firstChildElement(DISCARD_LARGE_ARRAYS_SETTINGS_TAG);
    data_type_configuration.discard_large_arrays = (discard_elem.attribute("value") == "true");

    return true;
}

void Configuration::save_default_settings(
        const QString& prefix) const
{
    // Get default Qt settings
    QSettings settings("eProsima", prefix);

    settings.setValue(prefix + MAX_ARRAY_SIZE_SETTINGS_TAG, data_type_configuration.max_array_size);
    settings.setValue(prefix + USE_HEADER_STAMP_SETTINGS_TAG, data_type_configuration.use_header_stamp);
    settings.setValue(prefix + DISCARD_LARGE_ARRAYS_SETTINGS_TAG, data_type_configuration.discard_large_arrays);
    settings.setValue(prefix + XML_DATATYPE_FILES_SETTINGS_TAG, xml_datatypes_files);
    settings.setValue(prefix + DOMAIN_ID_SETTINGS_TAG, domain_id);
    settings.setValue(prefix + SERVER_IP_SETTINGS_TAG, server_ip.c_str());
    settings.setValue(prefix + SERVER_PORT_SETTINGS_TAG, server_port);
}

void Configuration::load_default_settings(
        const QString& prefix)
{
    // Get default Qt settings
    QSettings settings("eProsima", prefix);

    data_type_configuration.max_array_size = settings.value(prefix + MAX_ARRAY_SIZE_SETTINGS_TAG,
                    data_type_configuration.max_array_size).toInt();
    data_type_configuration.use_header_stamp = settings.value(prefix + USE_HEADER_STAMP_SETTINGS_TAG,
                    data_type_configuration.use_header_stamp).toBool();
    data_type_configuration.discard_large_arrays = settings.value(prefix + DISCARD_LARGE_ARRAYS_SETTINGS_TAG,
                    data_type_configuration.discard_large_arrays).toBool();
    xml_datatypes_files = settings.value(prefix + XML_DATATYPE_FILES_SETTINGS_TAG, xml_datatypes_files).toStringList();
    domain_id = settings.value(prefix + DOMAIN_ID_SETTINGS_TAG, domain_id).toInt();
    server_ip = settings.value(prefix + SERVER_IP_SETTINGS_TAG, server_ip.c_str()).toString().toStdString();
    server_port = settings.value(prefix + SERVER_PORT_SETTINGS_TAG, server_port).toUInt();
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
