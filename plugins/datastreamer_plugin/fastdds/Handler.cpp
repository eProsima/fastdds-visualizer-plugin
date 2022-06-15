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
 * @file Handler.hpp
 */

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
    // Do nothing
}

Handler::~Handler()
{
    // Destroy internal participant explicitly before this object is destroyed
    reset_();
}


////////////////////////////////////////////////////
// INTERACTION METHODS
////////////////////////////////////////////////////

void Handler::connect_to_domain(
        const uint32_t domain)
{
    // Reset in case a Handler exist
    reset_();

    // Create participant
    participant_ = std::make_unique<Participant>(domain, discovery_database_, listener_);
}

void Handler::register_type_from_xml(
        const std::string& xml_path)
{
    if(participant_)
    {
        participant_->register_type_from_xml(xml_path);
    }
}

void Handler::create_subscriptions(
        const std::vector<std::string>& topic_names)
{
    if(participant_)
    {
        for (const auto& topic_name : topic_names)
        {
            participant_->create_subscription(topic_name);
        }
    }
}

std::shared_ptr<TopicDataBase> Handler::get_topic_data_base() const
{
    return discovery_database_;
}

void Handler::reset_()
{
    participant_.reset();

    clean_discovery_database_();
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
