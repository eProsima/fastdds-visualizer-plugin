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

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_PARTICIPANTS_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_PARTICIPANTS_HPP_

#include <function>
#include <memory>
#include <unordered_map>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include "Listener.hpp"
#include "ReaderHandler.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

/**
 * @brief This class handles every Fast DDS entity required.
 *
 * It create, manage and destroy every Fast DDS entity that the process requires to instantiate.
 * The discovery and user data received is transmitted through a Listener object.
 *
 * FUTURE WORK:
 * Use a specific thread to call callbacks instead of using Fast DDS thread
 */
class Participant : public eprosima::fastdds::dds::DomainParticipantListener
{
public:

    ////////////////////////////////////////////////////
    // CREATION & DESTRUCTION
    ////////////////////////////////////////////////////

    Participant(
        eprosima::fastdds::dds::DomainId_t domain_id,
        std::shared_ptr<Listener> listener);

    virtual ~Participant();


    ////////////////////////////////////////////////////
    // INTERACTION METHODS
    ////////////////////////////////////////////////////

    bool register_type_from_xml(
        const std::string& xml_path);

    void create_subscription(
        const std::string& topic_name);


    ////////////////////////////////////////////////////
    // LISTENER METHODS [ PARTICIPANT ]
    ////////////////////////////////////////////////////

    void on_publisher_discovery(
            eprosima::fastdds::dds::DomainParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override;

    void on_type_information_received(
            eprosima::fastdds::dds::DomainParticipant* participant,
            const fastrtps::string_255 topic_name,
            const fastrtps::string_255 type_name,
            const fastrtps::types::TypeInformation& type_information) override;

    void on_type_discovery(
            eprosima::fastdds::dds::DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::string_255& topic,
            const fastrtps::types::TypeIdentifier* identifier,
            const fastrtps::types::TypeObject* object,
            fastrtps::types::DynamicType_ptr dyn_type) override;


    ////////////////////////////////////////////////////
    // RETRIEVE VALUES METHODS
    ////////////////////////////////////////////////////

    void listener(const std::shared_ptr<Listener>& listener);

    std::shared_ptr<Listener> listener() const;

protected:

    ////////////////////////////////////////////////////
    // EXTERNAL EVENT METHODS
    ////////////////////////////////////////////////////

    void on_topic_discovery_(
        const std::string& topic_name,
        const std::string& type_name);

    void on_type_registration_(
        const std::string& type_name);


    ////////////////////////////////////////////////////
    // AUXILIAR METHODS
    ////////////////////////////////////////////////////

    void refresh_types_registered_();

    bool is_type_registered_(const std::string& type_name);



    ////////////////////////////////////////////////////
    // AUXILIAR STATIC METHODS
    ////////////////////////////////////////////////////

    static eprosima::fastdds::dds::DomainParticipantQos default_participant_qos_();

    static eprosima::fastdds::dds::SubscriberQos default_subscriber_qos_();

    static eprosima::fastdds::dds::TopicQos default_topic_qos_();

    static eprosima::fastdds::dds::DataReaderQos default_datareader_qos_();

    /**
     * @brief Get default mask
     *
     * Callbacks accepted by this mask:
     * - Every DomainParticipantListener callback (cannot be disabled)
     *
     * NOTE: it is important to have a mask, otherwise onDataOnReaders would hide on_data_available
     *
     * @return eprosima::fastdds::dds::StatusMask with callbacks needed
     */
    static eprosima::fastdds::dds::StatusMask default_listener_mask_();

    static std::function<void(ReaderHandler* )> reader_handler_deleter_();


    ////////////////////////////////////////////////////
    // INTERNAL VARIABLES
    ////////////////////////////////////////////////////

    std::unordered_map<std::string, std::string> topics_discovered_;

    std::unordered_map<std::string, eprosima::fastrtps::types::DynamicType_ptr> types_registered_;

    std::shared_ptr<Listener> listener_;

    // NOTE: this may not be needed
    eprosima::fastdds::dds::DomainId_t domain_id_;


    ////////////////////////////////////////////////////
    // FAST DDS POINTERS
    ////////////////////////////////////////////////////

    //! Internal DomainParticipant reference
    eprosima::fastdds::dds::DomainParticipant* participant_;
    //! Internal Subscriber reference (only one for every DataReader)
    eprosima::fastdds::dds::Subscriber* subscriber_;

    /**
     * Collection created in Participant indexed by topic name that contains
     * Reader, Topic and DataType so messages read is disengaged from this object
     */
    std::unordered_map<std::string, std::unique_ptr<ReaderHandler>> readers_;
}

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_PARTICIPANTS_HPP_
