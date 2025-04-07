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
 * @file Handler.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_HANDLER_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_HANDLER_HPP_

#include <memory>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>

#include "FastDdsListener.hpp"
#include "Participant.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

/**
 * @brief This class handles every Fast DDS entity required.
 *
 * It create, manage and destroy every Fast DDS entity that the process requires to instantiate.
 * The discovery and user data received is transmitted through a FastDdsListener object.
 *
 * FUTURE WORK:
 * Use a specific thread to call callbacks instead of using Fast DDS thread
 */
class Handler
{
public:

    ////////////////////////////////////////////////////
    // CREATION & DESTRUCTION
    ////////////////////////////////////////////////////

    Handler(
            FastDdsListener* listener);

    virtual ~Handler();


    ////////////////////////////////////////////////////
    // INTERACTION METHODS
    ////////////////////////////////////////////////////

    void connect_to_domain(
            const uint32_t domain);

    void connect_to_server(
        const uint32_t domain, const std::string& server_ip, unsigned int server_port);

    void register_type_from_xml(
            const std::string& xml_path);

    void create_subscription(
            const std::string& topic_name,
            const DataTypeConfiguration& data_type_configuration);

    std::shared_ptr<TopicDataBase> get_topic_data_base() const;

    void reset();

    std::vector<types::DatumLabel> numeric_data_series_names() const;

    std::vector<types::DatumLabel> string_data_series_names() const;

protected:

    ////////////////////////////////////////////////////
    // AUXILIAR INTERNAL METHODS
    ////////////////////////////////////////////////////

    void clean_discovery_database_();



    ////////////////////////////////////////////////////
    // INTERNAL VARIABLES
    ////////////////////////////////////////////////////

    std::shared_ptr<TopicDataBase> discovery_database_;

    std::unique_ptr<Participant> participant_;

    FastDdsListener* listener_;

    std::set<std::string> xml_data_types_paths_added_;
};

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_HANDLER_HPP_
