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
 * @file Participant.hpp
 */

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicDataFactory.hpp>
#include <fastdds/dds/xtypes/type_representation/ITypeObjectRegistry.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>

#include "Participant.hpp"
#include "utils/utils.hpp"
#include "utils/Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

////////////////////////////////////////////////////
// READERHANDLER DELETER
////////////////////////////////////////////////////

ReaderHandlerDeleter::ReaderHandlerDeleter(
        DomainParticipant* participant,
        Subscriber* subscriber)
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
        DomainId_t domain_id,
        std::shared_ptr<TopicDataBase> discovery_database,
        FastDdsListener* listener,
        bool use_discovery_server,
        const std::string& server_ip,
        const unsigned int& server_port)
    : listener_(listener)
    , discovery_database_(discovery_database)
{
    // TODO check entities are created correctly

    DomainParticipantQos participant_qos = default_participant_qos_();
    // Connect participat to server if discovery server is used
    if(use_discovery_server){
		// Define server locator
		eprosima::fastdds::rtps::Locator_t server_locator;
		eprosima::fastdds::rtps::IPLocator::setIPv4(server_locator, server_ip);
		server_locator.port = server_port;
		server_locator.kind = LOCATOR_KIND_UDPv4;
        // Set participant as SUPER CLIENT
        participant_qos.wire_protocol().builtin.discovery_config.discoveryProtocol = eprosima::fastdds::rtps::DiscoveryProtocol::SUPER_CLIENT;
        // Connect to remote server
        participant_qos.wire_protocol().builtin.discovery_config.m_DiscoveryServers.push_back(server_locator);
    }

    // Create Domain Participant
    participant_ = DomainParticipantFactory::get_instance()->create_participant(
        domain_id,
        participant_qos,
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
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
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
    if (RETCODE_OK != DomainParticipantFactory::get_instance()->load_XML_profiles_file(xml_path))
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
        const std::string& topic_name,
        const DataTypeConfiguration& data_type_configuration)
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
        WARNING("Topic " << topic_name << " has not been discovered not exist, so type unknown");
        throw InconsistencyException("Trying to create Data Reader in a non existing topic: " + topic_name);
    }

    DataTypeNameType type_name = discovery_database_->operator [](topic_name).first;
    DynamicType::_ref_type dyn_type;

    // Check if type is already registered
    if (discovery_database_->operator [](topic_name).second == false)
    {
        WARNING("Type " << topic_name << " has not been registered yet");
        throw InconsistencyException("Trying to create Data Reader in a non registered type: " + topic_name);
    }

    // Check if type is registered or not in participant. If not, register it
    if (!participant_->find_type(type_name))
    {
        // __FLAG__
        DEBUG("Type info not registered in participant for topic " << topic_name);
        //////////////////////////

        // Type information is available but not registered in participant
        // Types manually loaded (through XML file) are registered in participant when loaded, so this case is not possible
        // The only case is that the type info has been discovered automatically and the type Id has been saved in dyn_types_info_
        // Get TypeName and DataTypeId associated with topic name
        DataTypeId type_id = dyn_types_info_->operator [](topic_name).second;

        // Get Dyn Type and register discovered type
        xtypes::TypeObject type_object;
        if (RETCODE_OK != DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
                    type_id, type_object))
        {
            EPROSIMA_LOG_ERROR(PARTICIPANT, "Error getting type object for type " << type_name);
            return;
        }
        dyn_type = DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
            type_object)->build();
        TypeSupport dyn_type_support(new DynamicPubSubType(dyn_type));
        dyn_type_support.register_type(participant_);
    }
    else
    {
        // Type information is available and registered in participant. Create dyn_type for ReaderHandler
        // Two different cases:
        // 1. Type info has been discovered and registered by another participant. In this case, the type Id has been saved in dyn_types_info_
        // 2. Type info has been loaded manually (through XML file) and registered in participant. Info not saved in dyn_types_info_

        if (dyn_types_info_->find(topic_name) != dyn_types_info_->end())
        {
            DataTypeId type_id = dyn_types_info_->operator [](topic_name).second;
            xtypes::TypeObject type_object;
            if (RETCODE_OK != DomainParticipantFactory::get_instance()->type_object_registry().get_type_object(
                        type_id, type_object))
            {
                EPROSIMA_LOG_ERROR(PARTICIPANT, "Error getting type object for type " << type_name);
                return;
            }
            dyn_type = DynamicTypeBuilderFactory::get_instance()->create_type_w_type_object(
                type_object)->build();
        }
        else
        {
            DynamicTypeBuilder::_ref_type dyn_type_builder;
            ReturnCode_t ret = DomainParticipantFactory::get_instance()->get_dynamic_type_builder_from_xml_by_name(
                type_name, dyn_type_builder);

            if (RETCODE_OK != ret)
            {
                EPROSIMA_LOG_ERROR(PARTICIPANT, "Error getting DynamicTypeBuilder from XML");
                return;
            }
            dyn_type = dyn_type_builder->build();
        }
    }

    // Create topic
    eprosima::fastdds::dds::Topic* topic = participant_->create_topic(
        topic_name,
        type_name,
        default_topic_qos_());

    if (!topic)
    {
        EPROSIMA_LOG_ERROR(PARTICIPANT, "Error creating topic " << topic_name);
        return;
    }

    // Create datareader
    eprosima::fastdds::dds::DataReader* datareader = subscriber_->create_datareader(
        topic,
        default_datareader_qos_(),
        this);     // Mask not required

    if (!datareader)
    {
        EPROSIMA_LOG_ERROR(PARTICIPANT, "Error creating datareader for topic " << topic_name);
        return;
    }
    // Create Reader Handler with all this information and add it to readers
    // Create it with specific deleter for reader and topic
    ReaderHandlerReference new_reader(
        new ReaderHandler(
            topic,
            datareader,
            dyn_type,
            listener_,
            data_type_configuration),
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

void Participant::on_data_writer_discovery(
        DomainParticipant* participant,
        WriterDiscoveryStatus reason,
        const PublicationBuiltinTopicData& info,
        bool& should_be_ignored)
{
    should_be_ignored = false;

    // Only set as new topic discovered if it is ALIVE
    if (reason == WriterDiscoveryStatus::DISCOVERED_WRITER)
    {
        // Get Topic of DataWriter discovered and set it as discovered
        std::string topic_name = info.topic_name.to_string();
        std::string type_name = info.type_name.to_string();
        DEBUG(
            "DataWriter with guid " << info.guid << " discovered in topic : " <<
                topic_name << " [ " << type_name << " ]");

        if (!info.type_information.assigned())
        {
            // __FLAG__
            DEBUG("Type information not assigned for topic " << topic_name);
            /////////////////////////////
            // If type info is not assigned, check if it can be generated through a xml
            on_topic_discovery_(topic_name, type_name);
        }

        DataTypeId type_id = info.type_information.type_information.complete().typeid_with_size().type_id();

        // Set Topic as discovered. If it is not new nothing happen
        on_topic_discovery_(topic_name, type_name, type_id);
    }
}

////////////////////////////////////////////////////
// EXTERNAL EVENT METHODS
////////////////////////////////////////////////////

void Participant::on_topic_discovery_(
        const std::string& topic_name,
        const std::string& type_name)
{
    bool is_already_discovered = false;

    // Check if this topic has already been discovered
    auto it = discovery_database_->find(topic_name);
    if (it != discovery_database_->end())
    {
        is_already_discovered = true;

        // The topic had already been discovered, so not sending callback twice
        DEBUG(
            "Topic " << topic_name << " has already been discovered");
        return;
    }
    if (!is_already_discovered)
    {
        // Topic discovered without type information. Check if type information is available through xml and register it
        check_type_info(topic_name, type_name);
    }
    // Call listener callback to notify new topic
    if (listener_)
    {
        listener_->on_topic_discovery(topic_name, type_name);
    }
}

void Participant::on_topic_discovery_(
        const std::string& topic_name,
        const std::string& type_name,
        const DataTypeId& type_id)
{
    // TODO: check if mutex required
    // __FLAG__
    DEBUG("Calling on_topic_discovery with type id for topic " << topic_name);
    /////////////////
    bool is_already_discovered = false;

    // Check if this topic has already been discovered
    auto it = discovery_database_->find(topic_name);
    if (it != discovery_database_->end())
    {
        is_already_discovered = true;

        // The topic had already been discovered, so not sending callback twice
        DEBUG(
            "Topic " << topic_name << " has already been discovered");
        return;
    }

    if (!is_already_discovered)
    {
        // Add topic as discovered and save its type name and its type identifier to build DynamicType when DataReader is created
        // __FLAG__
        DEBUG("Topic " << topic_name << " discovered with type id");
        DEBUG("Updating databases...");
        DEBUG("...Updating discovery database...");
        discovery_database_->operator [](topic_name) = {type_name, true};
        DEBUG("...Updating dynamic types info database...");
        dyn_types_info_->operator [](topic_name) = {type_name, type_id};
        DEBUG("...Databases updated");
        /////////////////////////////////////////////
    }

    // Call listener callback to notify new topic
    if (listener_)
    {
        listener_->on_topic_discovery(topic_name, type_name);
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

ReturnCode_t Participant::get_type_support_from_xml_(
        const std::string& type_name,
        TypeSupport& type_support)
{
    DynamicTypeBuilder::_ref_type dyn_type_builder;
    ReturnCode_t ret = DomainParticipantFactory::get_instance()->get_dynamic_type_builder_from_xml_by_name(type_name,
                    dyn_type_builder);

    if (RETCODE_OK != ret)
    {
        EPROSIMA_LOG_ERROR(PARTICIPANT, "Error getting DynamicTypeBuilder from XML");
        return ret;
    }
    // TODO (Carlosespicur): Check if it can be done simpler
    DynamicType::_ref_type dyn_type = dyn_type_builder->build();
    TypeSupport dyn_type_type_support(new DynamicPubSubType(dyn_type));
    type_support = dyn_type_type_support;
    return RETCODE_OK;
}

void Participant::check_type_info(
        const std::string& topic_name,
        const std::string& type_name)
{
    // Check if type info has been loaded manually (though XML file). In this case, register it and update discovery database
    TypeSupport type_support;
    if (RETCODE_OK != get_type_support_from_xml_(type_name, type_support))
    {
        EPROSIMA_LOG_WARNING(PARTICIPANT, "type information of" << type_name << "is currently not available...");
        discovery_database_->operator [](topic_name) = {type_name, false};
        return;
    }
    if (!participant_->find_type(type_name))
    {
        // Type information is available and not registered in participant. Register it.
        DEBUG("Type info available. Registering type " << type_name << "in participant");
        discovery_database_->operator [](topic_name) = {type_name, true};
        type_support.register_type(participant_);
    }
}

void Participant::refresh_types_registered_()
{
    // Loop over every Topic discovered and check if type info is available
    // In case it is not, it may happen that the type info has been loaded manually (through XML profile) and registered in participant, so check in participant and
    // modify it if necessary
    for (auto const& [topic_name, topic_data_type_info] : *discovery_database_)
    {
        // Check if type info of a discovered type is currently available
        if (std::get<TypeInfoAvailable>(topic_data_type_info))
        {
            // Type info set as available. Two possibilities:
            // 1. Type info has been loaded manually (through XML profile) and registered in participant
            // 2. Type info has been discovered automatically. Type Id already saved in dyn_types_info_
            // Nothing to do in both cases
            continue;
        }
        else
        {
            // Type info is set as not available. Check if it has been loaded manually and update discovery database in this case
            check_type_info(topic_name, std::get<DataTypeNameType>(topic_data_type_info));
        }

        // Call listener callback to notify new topic
        // NOTE: This is necessary to refresh the list of topics in the UI
        if (listener_)
        {
            listener_->on_topic_discovery(topic_name, std::get<DataTypeNameType>(topic_data_type_info));
        }
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

    // Set Metadata
    qos.properties().properties().emplace_back("fastdds.application.id", "FASTDDS_VISUALIZER", true);
    qos.properties().properties().emplace_back("fastdds.application.metadata", "", true);
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
