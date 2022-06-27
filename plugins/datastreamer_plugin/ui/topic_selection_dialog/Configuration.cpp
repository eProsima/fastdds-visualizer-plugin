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
 * @file Configuration.cpp
 */

#include "Configuration.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

////////////////////
// Save Load configuration
bool Configuration::xmlSaveState(
        QDomDocument& doc,
        QDomElement& plugin_elem) const
{
    DEBUG("Saving XML configuration state");

    QDomElement max_elem = doc.createElement(MAX_ARRAY_SIZE_SETTINGS_TAG);
    max_elem.setAttribute("value", QString::number(max_array_size));
    parent_elem.appendChild(max_elem);

    QDomElement stamp_elem = doc.createElement(USE_HEADER_STAMP_SETTINGS_TAG);
    stamp_elem.setAttribute("value", use_header_stamp ? "true" : "false");
    parent_elem.appendChild(stamp_elem);

    QDomElement discard_elem = doc.createElement(DISCARD_LARGE_ARRAYS_SETTINGS_TAG);
    discard_elem.setAttribute("value", discard_large_arrays ? "true" : "false");
    parent_elem.appendChild(discard_elem);
}

bool Configuration::xmlLoadState(
        const QDomElement& parent_element)
{
    DEBUG("Loading XML configuration state");

    QDomElement max_elem = parent_element.firstChildElement(MAX_ARRAY_SIZE_SETTINGS_TAG);
    max_array_size = max_elem.attribute("value").toInt();

    QDomElement stamp_elem = parent_element.firstChildElement(USE_HEADER_STAMP_SETTINGS_TAG);
    use_header_stamp = (stamp_elem.attribute("value") == "true");

    QDomElement discard_elem = parent_element.firstChildElementDISCARD_LARGE_ARRAYS_SETTINGS_TAG);
    discard_large_arrays = (discard_elem.attribute("value") == "true");
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
