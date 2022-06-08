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
 * @file Participant.hpp
 */

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastrtps/types/DynamicDataHelper.hpp>

#include "Participant.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {


////////////////////////////////////////////////////
// CREATION & DESTRUCTION
////////////////////////////////////////////////////

Participant::Participant(
        eprosima::fastdds::dds::DomainId_t domain_id,
        std::shared_ptr<Listener> listener)
    : listener_(listener)
    , domain_id_(domain_id)
{
    // Create Domain Participant
    participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
            domain_id_,
            default_participant_qos_(),
            this,
            default_listener_mask_());

    // Create Subscriber without listener
    subscriber_ = participant_->create_subscriber(
            default_subscriber_qos_());
}

Participant::~Participant()
{
    // Eliminate references to any Dynamic Data Type that may have been created
    types_registered_.clear();

    // Eliminate reader handlers (smart ptr deleter will handle it)
    readers_.clear();

    // If participant exist, destroy it and its subentities
    if (participant_)
    {
        // If subscriber exist, destroy it and its subentities
        if (subscriber_)
        {
            // Reader and Topic are destroyed by smart ptr
            // TODO: check if stop is required before

            // Destroy subscriber
            participant_->delete_subscriber(subscriber_);
        }

        // Destroy participant
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    // The rest of maps and variables are destroyed by themselves
}


////////////////////////////////////////////////////
// INTERACTION METHODS
////////////////////////////////////////////////////

// TODO: check if error handling by bool or exception
bool Participant::register_type_from_xml(
        const std::string& xml_path)
{
    eprosima::fastrtps::xmlparser::XMLP_ret ret =
        eprosima::fastrtps::xmlparser::XMLProfileManager::loadXMLFile(xml_path);
    if (eprosima::fastrtps::xmlparser::XMLP_ret::XML_OK != ret)
    {
        logWarning(PLOTJUGGLER_FASTDDS, "Error loading XML file: " << xml_path);
        return false;
    }

    // Set these types as registered
    refresh_types_registered_();
}

void Participant::create_subscription(
        const std::string& topic_name)
{
    // TODO: check if mutex required

    // Check datareader does not exist yet
    if (readers_.find(topic_name) != readers_.end())
    {
        logWarning(PLOTJUGGLER_FASTDDS, "Datareader already exists for topic: " << topic_name);
        return;
    }

    // Check the Topic exist, so the type name is known
    auto topic_type = topics_discovered_.find(topic_name);
    if (topic_type == topics_discovered_.end())
    {
        logWarning(
            PLOTJUGGLER_FASTDDS,
            "Topic " << topic_name << " has not been discovered not exist, so type unknowon");
        return;
    }

    // Get TypeName associated with topic name
    std::string type_name = topics_discovered_[topic_name];

    // Create topic
    eprosima::fastdds::dds::Topic* topic = participant_->create_topic(
            topic_name,
            topic_type->second,
            default_topic_qos_());

    // Create datareader
    eprosima::fastdds::dds::DataReader* datareader = subscriber_->create_datareader(
            topic,
            default_datareader_qos_(),
            this); // Mask not required

    // Get Dyn Type for type
    // This could not fail, as we know type is registered
    eprosima::fastrtps::types::DynamicType_ptr dyn_type = types_registered_[type_name];

    // Create Reader Handler with all this information and add it to readers
    // Create it with specific deleter for reader and topic
    std::unique_ptr<ReaderHandler> new_reader(
        new ReaderHandler(
            topic,
            datareader,
            dyn_type,
            listener_),
        reader_handler_deleter_()
    );

    // Insert this new Reader Handler indexed by topic name
    readers_.insert(
        std::make_pair(
            topic_name,
            std::move(new_reader)));
}


////////////////////////////////////////////////////
// LISTENER METHODS [ PARTICIPANT ]
////////////////////////////////////////////////////

void Participant::on_publisher_discovery(
        eprosima::fastdds::dds::DomainParticipant* participant,
        fastrtps::rtps::WriterDiscoveryInfo&& info)
{
    // Get Topic of DataWriter discovered and set it as discovered
    std::string topic_name = info.info.topicName().to_string();
    std::string type_name = info.info.typeName().to_string();

    logInfo(
        PLOTJUGGLER_FASTDDS,
        "DataWriter with guid " << info.info.guid() << " discovered in topic : " <<
            topic_name << " [ " << type_name << " ]");

    // Set Topic as discovered. If it is not new nothing happen
    on_topic_discovery_(topic_name, type_name);
}

void Participant::on_type_information_received(
        eprosima::fastdds::dds::DomainParticipant* participant,
        const fastrtps::string_255 topic_name,
        const fastrtps::string_255 type_name,
        const fastrtps::types::TypeInformation& type_information)
{
    // TODO (check if needed)
}

void Participant::on_type_discovery(
        eprosima::fastdds::dds::DomainParticipant* participant,
        const fastrtps::rtps::SampleIdentity& request_sample_id,
        const fastrtps::string_255& topic,
        const fastrtps::types::TypeIdentifier* identifier,
        const fastrtps::types::TypeObject* object,
        fastrtps::types::DynamicType_ptr dyn_type)
{
    // In case this callback is sent, it means that the type is already registered, so notify
    on_type_registration_(dyn_type->get_name());
}


////////////////////////////////////////////////////
// RETRIEVE VALUES METHODS
////////////////////////////////////////////////////

void Participant::listener(const std::shared_ptr<Listener>& listener)
{
    listener_ = listener;

    // Change listener in every reader handler
    for (auto& reader : readers_)
    {
        reader.second->listener(listener_);
    }
}

std::shared_ptr<Listener> Participant::listener() const
{
    return listener_;
}

////////////////////////////////////////////////////
// EXTERNAL EVENT METHODS
////////////////////////////////////////////////////

void Participant::on_topic_discovery_(
        const std::string& topic_name,
        const std::string& type_name)
{
    // TODO: check if mutex required

    // Check if this topic has already been discovered
    if (topics_discovered_.find(topic_name) != topics_discovered_.end())
    {
        logInfo(
            PLOTJUGGLER_FASTDDS,
            "Topic " << topic_name << " has already been discovered");
        return;
    }

    // Add topic as discovered
    topics_discovered_[topic_name] = type_name;

    // Check if type is registered in Participant
    bool is_registered = is_type_registered_(type_name);

    // Call listener callback to notify new topic
    if (listener_)
    {
        listener_->on_topic_discovery(topic_name, type_name, is_registered);
    }
}

void Participant::on_type_registration_(
        const std::string& type_name)
{
    // TODO: check if mutex required

    // Check type does not exist yet
    if (types_registered_.find(type_name) != types_registered_.end())
    {
        logInfo(PLOTJUGGLER_FASTDDS, "Type already registered: " << type_name);
        return;
    }

    eprosima::fastrtps::types::DynamicType_ptr dyn_type =
            eprosima::fastrtps::xmlparser::XMLProfileManager::getDynamicTypeByName(type_name)->build();

    // Register type inside this object
    types_registered_[type_name] = dyn_type;

    // If there is a topic with this type, callback must be called
    for (auto& topic : topics_discovered_)
    {
        if (topic.second == type_name)
        {
            // Call listener callback to notify new topic
            if (listener_)
            {
                listener_->on_topic_discovery(topic.first, type_name, true);
            }
        }
    }
}

void Participant::refresh_types_registered_()
{
    // TODO: check if mutex required

    // Loop over every topic discovered
    for (auto& topic : topics_discovered_)
    {
        // Call on_type_registration for each type
        // In case it has already been registered, nothing will happen.
        // Otherwise it will be registered locally and send callback if needed
        on_type_registration_(topic.second);
    }
}


////////////////////////////////////////////////////
// AUXILIAR STATIC METHODS
////////////////////////////////////////////////////

eprosima::fastdds::dds::DomainParticipantQos Participant::default_participant_qos_()
{
    eprosima::fastdds::dds::DomainParticipantQos qos =
        eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;

    // Set Generic Name
    qos.name("PlotJuggler_FastDDSPlugin_Participant");

    // Set to be used as a client type lookup
    qos.wire_protocol().builtin.typelookup_config.use_client = true;

    return qos;
}

eprosima::fastdds::dds::SubscriberQos Participant::default_subscriber_qos_()
{
    eprosima::fastdds::dds::SubscriberQos qos =
        eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;

    return qos;
}

eprosima::fastdds::dds::DataReaderQos Participant::default_datareader_qos_()
{
    eprosima::fastdds::dds::DataReaderQos qos =
        eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;

    return qos;
}

eprosima::fastdds::dds::TopicQos Participant::default_topic_qos_()
{
    eprosima::fastdds::dds::TopicQos qos =
        eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;

    return qos;
}

eprosima::fastdds::dds::StatusMask Participant::default_listener_mask_()
{
    eprosima::fastdds::dds::StatusMask mask;

    // Erase all (DomainParticipant ones are always active)
    mask.none();

    return mask;
}

std::function<void(ReaderHandler* )> reader_handler_deleter_()
{
    return
        [this]
        (ReaderHandler* reader)
        {
            // Stop data gathering
            reader->stop();
            // Delete reader
            subscriber_->delete_datareader(reader->reader_);
            // Delete topic
            participant_->delete_topic(reader->topic_);
        };
}

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
