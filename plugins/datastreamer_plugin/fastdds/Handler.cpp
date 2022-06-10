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
        std::shared_ptr<Listener> listener)
    : listener_(listener)
{
    // Do nothing
}

Handler::~Handler()
{
    // Destroy internal participant explicitly before this object is destroyed
    // In case it is not constructed, nothing happens
    participant_.reset();

    // Listener does not matter if it is destroyed before or after (and it may survive after this object)
}


////////////////////////////////////////////////////
// INTERACTION METHODS
////////////////////////////////////////////////////

void Handler::connect_to_domain(
        const uint32_t domain)
{
    // Reset in case a Handler exist
    participant_.reset();

    // Create participant
    participant_ = std::make_unique<Participant>(domain, listener_);

    // Register listener
    participant_->listener(listener_);
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


////////////////////////////////////////////////////
// RETRIEVE VALUES METHODS
////////////////////////////////////////////////////

void Handler::listener(const std::shared_ptr<Listener>& listener)
{
    listener_ = listener;
}

std::shared_ptr<Listener> Handler::listener() const
{
    return listener_;
}


} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
