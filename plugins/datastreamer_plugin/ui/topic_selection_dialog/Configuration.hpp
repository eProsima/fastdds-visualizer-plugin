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
 * @file Configuration.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_

#include <QStringList>
#include <QSettings>
#include <QDomDocument>

namespace eprosima {
namespace plotjuggler {
namespace ui {

/**
 * @brief This class handles every Fast DDS entity required.
 *
 * It create, manage and destroy every Fast DDS entity that the process requires to instantiate.
 * The discovery and user data received is transmitted through a UiListener object.
 *
 * FUTURE WORK:
 * Use a specific thread to call callbacks instead of using Fast DDS thread
 */
struct Configuration
{

    ////////////////////
    // Topics
    QStringList topics;  // Empty in initialization

    ////////////////////
    // Max array size
    unsigned max_array_size = 1000;
    bool use_header_stamp = true;
    bool discard_large_arrays = false;

    ////////////////////
    // DDS Configuration
    uint32_t domain_id = 0;
    QStringList xml_datatypes_files;  // Empty in initialization

    ////////////////////
    // Advance options
    bool boolean_strings_to_number = false;

    ////////////////////
    // Save Load configuration
    void xmlSaveState(
            QDomDocument& doc,
            QDomElement& plugin_elem) const;
    void xmlLoadState(
            const QDomElement& parent_element);

    void saveToSettings(
            QSettings& setting,
            QString prefix) const;
    void loadFromSettings(
            const QSettings& settings,
            QString prefix);

};

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_UI_TOPICSELECTIONDIALOG_CONFIGURATION_HPP_
