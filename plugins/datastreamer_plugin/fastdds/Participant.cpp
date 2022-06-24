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
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/TypeObjectFactory.h>

#include "Participant.hpp"
#include "utils/utils.hpp"
#include "utils/Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

////////////////////////////////////////////////////
// READERHANDLER DELETER
////////////////////////////////////////////////////

ReaderHandlerDeleter::ReaderHandlerDeleter(
        eprosima::fastdds::dds::DomainParticipant* participant,
        eprosima::fastdds::dds::Subscriber* subscriber)
    : participant_(participant)
    , subscriber_(subscriber)
{
}

void ReaderHandlerDeleter::operator ()(
        ReaderHandler* reader) const
{
    // Stop data gathering
    reader->stop();
    // Delete reader
    subscriber_->delete_datareader(reader->reader_);
    // Delete topic
    participant_->delete_topic(reader->topic_);
}

////////////////////////////////////////////////////
// CREATION & DESTRUCTION
////////////////////////////////////////////////////

Participant::Participant(
        eprosima::fastdds::dds::DomainId_t domain_id,
        std::shared_ptr<TopicDataBase> discovery_database,
        FastDdsListener* listener)
    : listener_(listener)
    , discovery_database_(discovery_database)
{
    // TODO check entities are created correctly

    // Create Domain Participant
    participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(
        domain_id,
        default_participant_qos_(),
        this,
        default_listener_mask_());

    if (!participant_)
    {
        throw InitializationException("Error creating Domain Participant");
    }

    DEBUG("Participant created in domain " << domain_id << " with guid: " << participant_->guid());

    // Create Subscriber without listener
    subscriber_ = participant_->create_subscriber(
        default_subscriber_qos_());

    if (!subscriber_)
    {
        // Delete participant
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
        participant_ = nullptr;

        throw InitializationException("Error creating Subscriber");
    }
}

Participant::~Participant()
{
    DEBUG("Destroying Participant");

    // If participant exist, destroy it and its subentities
    if (participant_)
    {
        // If subscriber exist, destroy it and its subentities
        if (subscriber_)
        {
            // Eliminate reader handlers (smart ptr deleter will handle it)
            readers_.clear();

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
        WARNING("Error loading XML file: " << xml_path);
        throw IncorrectParamException("Failed reading XML file: " + xml_path);
    }

    DEBUG("Registered types in xml file: " << xml_path);

    // Loading an xml does not retrieve the types loaded.
    // Thus, it is necessary to loop over all types discovered and check if they are registered already
    refresh_types_registered_();

    return true;
}

void Participant::create_subscription(
        const std::string& topic_name)
{
    // TODO: check if mutex required
    DEBUG("Creating subscription for topic: " << topic_name);

    // Check datareader does not exist yet
    if (readers_.find(topic_name) != readers_.end())
    {
        WARNING("Datareader already exists for topic: " << topic_name);
        throw InconsistencyException("Trying to create Data Reader again in topic: " + topic_name);
    }

    // Check the Topic exist, so the type name is known
    auto topic_type = discovery_database_->find(topic_name);
    if (topic_type == discovery_database_->end())
    {
        WARNING("Topic " << topic_name << " has not been discovered not exist, so type unknowon");
        throw InconsistencyException("Trying to create Data Reader in a non existing topic: " + topic_name);
    }

    // Get TypeName associated with topic name
    std::string type_name = discovery_database_->operator [](topic_name).first;

    // Create topic
    eprosima::fastdds::dds::Topic* topic = participant_->create_topic(
        topic_name,
        type_name,
        default_topic_qos_());

    // Create datareader
    eprosima::fastdds::dds::DataReader* datareader = subscriber_->create_datareader(
        topic,
        default_datareader_qos_(),
        this);     // Mask not required

    // Get Dyn Type for type
    // This could not fail, as we know type is registered
    eprosima::fastrtps::types::DynamicType_ptr dyn_type = get_type_registered_(type_name);

    // Create Reader Handler with all this information and add it to readers
    // Create it with specific deleter for reader and topic
    ReaderHandlerReference new_reader(
        new ReaderHandler(
            topic,
            datareader,
            dyn_type,
            listener_),
        ReaderHandlerDeleter(participant_, subscriber_)
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
    // Only set as new topic discovered if it is ALIVE
    if (info.status == eprosima::fastrtps::rtps::WriterDiscoveryInfo::DISCOVERY_STATUS::DISCOVERED_WRITER)
    {
        // Get Topic of DataWriter discovered and set it as discovered
        std::string topic_name = info.info.topicName().to_string();
        std::string type_name = info.info.typeName().to_string();

        DEBUG(
            "DataWriter with guid " << info.info.guid() << " discovered in topic : " <<
                topic_name << " [ " << type_name << " ]");

        // Set Topic as discovered. If it is not new nothing happen
        on_topic_discovery_(topic_name, type_name);
    }
}

void Participant::on_type_information_received(
        eprosima::fastdds::dds::DomainParticipant*,
        const fastrtps::string_255 topic_name,
        const fastrtps::string_255 type_name,
        const fastrtps::types::TypeInformation& type_information)
{
    DEBUG("Type Information received: " << type_name.to_string() << " in topic: " << topic_name.to_string());

    // Prepare callback that will be executed after registering type
    std::function<void(const std::string&, const eprosima::fastrtps::types::DynamicType_ptr)> callback(
        [this, topic_name]
            (const std::string&, const eprosima::fastrtps::types::DynamicType_ptr type)
        {
            DEBUG(
                "Type discovered by lookup info: " << type->get_name() << " in topic: " << topic_name.to_string());
            this->on_topic_discovery_(topic_name.to_string(), type->get_name());
        });

    // Registering type and creating reader
    participant_->register_remote_type(
        type_information,
        type_name.to_string(),
        callback);
}

void Participant::on_type_discovery(
        eprosima::fastdds::dds::DomainParticipant* participant,
        const fastrtps::rtps::SampleIdentity& request_sample_id,
        const fastrtps::string_255& topic,
        const fastrtps::types::TypeIdentifier* identifier,
        const fastrtps::types::TypeObject* object,
        fastrtps::types::DynamicType_ptr dyn_type)
{
    if (!dyn_type)
    {
        // Fast DDS may call this callback with a nullptr in dyn_type because of reasons. Avoid break.
        WARNING("on_type_discovery callback called with nullptr dyn type");
        return;
    }

    DEBUG("TypeObject discovered: " << dyn_type->get_name() << " for topic: " << topic.to_string());

    // TOOD study this
    // In case of complex data types, registering here means that the data type will be incorrectly registered
    // because the internal data will be received and registered faster than lookup service, which produces an error
    // when registering type:
    //  logError(PARTICIPANT, "Another type with the same name Struct_TypeIntrospectionExample is already registered.");
    // WORKAROUND: only use TypeLookup service to get Type Information and forget about the TypeObject
    // For the future: it seems like the error appears when registering the type from here because this callback is
    // erroneous, so it can be checked wether this type has already been registered. Be careful because it should
    // not only check it has been registered but it has been discovered by the Service, that may be in process of
    // registering when this callback arrives

    // Create TypeSupport and register it
    // eprosima::fastdds::dds::TypeSupport(
    //     new eprosima::fastrtps::types::DynamicPubSubType(dyn_type)).register_type(participant_);

    // // In case this callback is sent, it means that the type is already registered, so notify
    // // TODO in future it would be better to update every topic in this type name, and not just the one calling here
    // on_topic_discovery_(topic.to_string(), dyn_type->get_name());
}

////////////////////////////////////////////////////
// EXTERNAL EVENT METHODS
////////////////////////////////////////////////////

void Participant::on_topic_discovery_(
        const std::string& topic_name,
        const std::string& type_name)
{
    // TODO: check if mutex required

    // Check if type is registered in Participant
    bool is_registered = is_type_registered_in_participant_(type_name);
    bool is_already_discovered = false;

    // Check if this topic has already been discovered
    auto it = discovery_database_->find(topic_name);
    if (it != discovery_database_->end())
    {
        is_already_discovered = true;

        // If the topic is already discovered, check the registered type
        if (it->second.second)
        {
            // The topic had already been discovered and registered, so not sending callback twice
            DEBUG(
                "Topic " << topic_name << " has already been discovered");
            return;
        }
    }

    if (!is_already_discovered)
    {
        // Add topic as discovered
        discovery_database_->operator [](topic_name) = {type_name, is_registered};
    }

    // Call listener callback to notify new topic
    if (listener_)
    {
        listener_->on_topic_discovery(topic_name, type_name, is_registered);
    }
}

////////////////////////////////////////////////////
// RETRIEVE INFORMATION METHODS
////////////////////////////////////////////////////

std::vector<types::DatumLabel> Participant::numeric_data_series_names() const
{
    std::vector<types::DatumLabel> names;

    for (const auto& reader : readers_)
    {
        for (const auto& series : reader.second->numeric_data_series_names())
        {
            names.push_back(series);
        }
    }

    return names;
}

std::vector<types::DatumLabel> Participant::string_data_series_names() const
{
    std::vector<types::DatumLabel> names;

    for (const auto& reader : readers_)
    {
        for (const auto& series : reader.second->string_data_series_names())
        {
            names.push_back(series);
        }
    }

    return names;
}

////////////////////////////////////////////////////
// AUXILIAR METHODS
////////////////////////////////////////////////////

void Participant::refresh_types_registered_()
{
    // Loop over every Topic discovered and check if type is registered
    // In case it is not, it may happen that the type is registered now, so check in participant and
    // modify it if necessary
    for (auto& topic_entry : *discovery_database_)
    {
        // Check if type is registered in database
        if (topic_entry.second.second)
        {
            // Type is registered, so nothing to do
            continue;
        }
        else
        {
            // If already registered, add it to database and notify listener by own topic discovery callback
            if (is_type_registered_in_participant_(topic_entry.second.first))
            {
                // NOTE Do not modify the discovery database, as it must be consistent with the data sent in
                // callbacks and not with the internal data.
                // Set as discovered, so listener is called
                on_topic_discovery_(topic_entry.first, topic_entry.second.first);
            }
        }
    }
}

bool Participant::is_type_registered_in_participant_(
        const std::string& type_name)
{
    // Check type is registered in Participant
    if (participant_->find_type(type_name) != nullptr)
    {
        return true;
    }
    else
    {
        // It may happen that type is registered in XML and not in Participant
        // If so, register it in Participant
        if (is_type_registered_in_xml_(type_name))
        {
            // Create TypeSupport and register it
            eprosima::fastdds::dds::TypeSupport(
                new eprosima::fastrtps::types::DynamicPubSubType(
                    get_type_registered_(type_name))).register_type(participant_);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Participant::is_type_registered_in_xml_(
        const std::string& type_name)
{
    return nullptr != eprosima::fastrtps::xmlparser::XMLProfileManager::getDynamicTypeByName(type_name);
}

eprosima::fastrtps::types::DynamicType_ptr Participant::get_type_registered_(
        const std::string& type_name)
{
    // Get DynamicType builder
    auto builder = eprosima::fastrtps::xmlparser::XMLProfileManager::getDynamicTypeByName(type_name);

    // If not builder associated, the type does not exist
    if (!builder)
    {
        // Check if it could be generated
        // This case is when it has not been registered by XML
        auto type_object =
                eprosima::fastrtps::types::TypeObjectFactory::get_instance()->get_type_object(type_name,
                        true);
        if (!type_object)
        {
            throw IncorrectParamException("Dynamic type not registered");
        }

        auto type_id =
                eprosima::fastrtps::types::TypeObjectFactory::get_instance()->get_type_identifier(type_name,
                        true);
        if (!type_id)
        {
            throw IncorrectParamException("Dynamic type not registered");
        }

        auto dyn_type = eprosima::fastrtps::types::TypeObjectFactory::get_instance()->build_dynamic_type(type_name,
                        type_id,
                        type_object);

        return dyn_type;
    }
    else
    {
        return builder->build();
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

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
