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

#include <fastdds/dds/log/Log.hpp>

#include "Handler.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

////////////////////////////////////////////////////
// CREATION & DESTRUCTION
////////////////////////////////////////////////////

Handler::Handler(
        FastDdsListener* listener)
    : listener_(listener)
    , discovery_database_(std::make_shared<TopicDataBase>())
{
    // TOOD remove
    // Activate fast dds warning log
    eprosima::fastdds::dds::Log::SetVerbosity(eprosima::fastdds::dds::Log::Kind::Warning);
    // Do nothing
}

Handler::~Handler()
{
    // Destroy internal participant explicitly before this object is destroyed
    reset();
}

////////////////////////////////////////////////////
// INTERACTION METHODS
////////////////////////////////////////////////////

void Handler::connect_to_domain(
        const uint32_t domain)
{
    // Reset in case a Handler exist
    reset();

    // Create participant
    participant_ = std::make_unique<Participant>(domain, discovery_database_, listener_);
}

void Handler::connect_to_server(
        const uint32_t domain, const std::string& server_ip, unsigned int server_port)
{
    // Reset in case a Handler exist
    reset();

    // Create participant using discovery server
    participant_ = std::make_unique<Participant>(domain, discovery_database_, listener_, true, server_ip, server_port);
}

void Handler::register_type_from_xml(
        const std::string& xml_path)
{
    if (participant_)
    {
        // Only register xml if it has not been done before
        if (xml_data_types_paths_added_.find(xml_path) == xml_data_types_paths_added_.end())
        {
            participant_->register_type_from_xml(xml_path);
            xml_data_types_paths_added_.insert(xml_path);
        }
    }
    else
    {
        WARNING("Trying to add xml from a non-connected handler");
    }
}

void Handler::create_subscription(
        const std::string& topic_name,
        const DataTypeConfiguration& data_type_configuration)
{
    if (participant_)
    {
        participant_->create_subscription(topic_name, data_type_configuration);
    }
}

std::shared_ptr<TopicDataBase> Handler::get_topic_data_base() const
{
    return discovery_database_;
}

void Handler::reset()
{
    participant_.reset();

    clean_discovery_database_();
}

std::vector<types::DatumLabel> Handler::numeric_data_series_names() const
{
    if (participant_)
    {
        return participant_->numeric_data_series_names();
    }
    else
    {
        return std::vector<types::DatumLabel>();
    }
}

std::vector<types::DatumLabel> Handler::string_data_series_names() const
{
    if (participant_)
    {
        return participant_->string_data_series_names();
    }
    else
    {
        return std::vector<types::DatumLabel>();
    }
}

////////////////////////////////////////////////////
// AUXILIAR INTERNAL METHODS
////////////////////////////////////////////////////

void Handler::clean_discovery_database_()
{
    discovery_database_->clear();
}

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
