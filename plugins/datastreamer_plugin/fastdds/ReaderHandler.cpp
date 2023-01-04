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
 * @file ReaderHandler.hpp
 */

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicDataFactory.h>

#include "ReaderHandler.hpp"
#include "utils/utils.hpp"
#include "utils/dynamic_types_utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {


////////////////////////////////////////////////////
// CREATION & DESTRUCTION
////////////////////////////////////////////////////

ReaderHandler::ReaderHandler(
        eprosima::fastdds::dds::Topic* topic,
        eprosima::fastdds::dds::DataReader* datareader,
        eprosima::fastrtps::types::DynamicType_ptr type,
        FastDdsListener* listener,
        const DataTypeConfiguration& data_type_configuration)
    : topic_(topic)
    , reader_(datareader)
    , type_(type)
    , listener_(listener)
    , stop_(false)
{
    // Create data so it is not required to create it each time and avoid reallocation if possible
    data_ = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(type_);

    // Create the static structures to store the data introspection information AND the data itself
    utils::get_introspection_type_names(
        topic_name(),
        type_,
        data_type_configuration,
        numeric_data_info_,
        string_data_info_);

    // Create the data structures so they are not copied in the future
    for (const auto& info : numeric_data_info_)
    {
        numeric_data_.push_back({ std::get<0>(info), 0});
    }
    for (const auto& info : string_data_info_)
    {
        string_data_.push_back({ std::get<0>(info), "-"});
    }

    DEBUG("Reader created in topic: " << topic_name() << " with types: ");
    for (const auto& info : numeric_data_info_)
    {
        DEBUG("\tNumeric: " << std::get<0>(info));
    }
    for (const auto& info : string_data_info_)
    {
        DEBUG("\tString: " << std::get<0>(info));
    }

    // Set this object as this reader's listener
    reader_->set_listener(this);
}

ReaderHandler::~ReaderHandler()
{
    // Stop the reader
    stop();

    // Delete created data 
    eprosima::fastrtps::types::DynamicDataFactory::get_instance()->delete_data(data_);
}

////////////////////////////////////////////////////
// INTERACTION METHODS
////////////////////////////////////////////////////

void ReaderHandler::stop()
{
    // Stop the reader
    stop_ = true;
    reader_->set_listener(nullptr);
}

////////////////////////////////////////////////////
// LISTENER METHODS [ DATAREADER ]
////////////////////////////////////////////////////

void ReaderHandler::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    eprosima::fastdds::dds::SampleInfo info;
    eprosima::fastrtps::types::ReturnCode_t read_ret =
            eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK;

    // Read Data from reader while there is data available and not should stop
    while (!stop_ && read_ret == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK)
    {
        // Read next data
        read_ret = reader->take_next_sample(data_, &info);

        // If data has been read
        if (read_ret == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK &&
                info.instance_state == eprosima::fastdds::dds::InstanceStateKind::ALIVE_INSTANCE_STATE)
        {
            // Get timestamp
            double timestamp = utils::get_timestamp_seconds_numeric_value(info.reception_timestamp);

            // Get data in already created structures
            utils::get_introspection_numeric_data(
                numeric_data_info_,
                data_,
                numeric_data_);

            utils::get_introspection_string_data(
                string_data_info_,
                data_,
                string_data_);

            // Get value maps from data and send callback if there are data
            if (!numeric_data_.empty())
            {
                listener_->on_double_data_read(
                    numeric_data_,
                    timestamp);
            }

            // Same for strings
            if (!string_data_.empty())
            {
                listener_->on_string_data_read(
                    string_data_,
                    timestamp);
            }
        }
    }
}

////////////////////////////////////////////////////
// VALUES METHODS
////////////////////////////////////////////////////

const std::string& ReaderHandler::topic_name() const
{
    return topic_->get_name();
}

const std::string& ReaderHandler::type_name() const
{
    return topic_->get_type_name();
}

////////////////////////////////////////////////////
// AUXILIAR STATIC METHODS
////////////////////////////////////////////////////

eprosima::fastdds::dds::StatusMask ReaderHandler::default_listener_mask_()
{
    // Start from all bits set to 0
    eprosima::fastdds::dds::StatusMask mask = eprosima::fastdds::dds::StatusMask::none();

    // Only listen this callback (and the DomainParticipantListener ones)
    mask << eprosima::fastdds::dds::StatusMask::data_available();

    return mask;
}

std::vector<std::string> ReaderHandler::numeric_data_series_names() const
{
    return utils::get_introspection_type_names(numeric_data_info_);
}

std::vector<std::string> ReaderHandler::string_data_series_names() const
{
    return utils::get_introspection_type_names(string_data_info_);
}

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
