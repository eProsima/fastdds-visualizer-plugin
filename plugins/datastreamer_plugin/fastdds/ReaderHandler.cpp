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
 * @file ReaderHandler.hpp
 */

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicDataFactory.h>

#include "ReaderHandler.hpp"
#include "utils/utils.hpp"

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
        std::shared_ptr<Listener> listener)
    : topic_(topic)
    , reader_(datareader)
    , type_(type)
    , listener_(listener)
    , stop_(false)
{
    // Create data so it is not required to create it each time and avoid reallocation if possible
    data_ = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(type_);
}

ReaderHandler::~ReaderHandler()
{
    // Stop the reader
    stop();
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
    while(!stop_ && read_ret == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK)
    {
        // Read next data
        read_ret = reader->take_next_sample(data_.get(), &info);

        // If data has been read
        if (read_ret == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK &&
            info.instance_state == eprosima::fastdds::dds::InstanceStateKind::ALIVE_INSTANCE_STATE)
            {
                // Get timestamp
                std::string timestamp(utils::get_timestamp_string(info.reception_timestamp));

                // Get value maps from data and send callback if there are data
                auto numeric_values = numeric_data_();
                if (!numeric_values.empty())
                {
                    listener_->on_double_data_read(
                        numeric_values,
                        timestamp);
                }

                // Same for strings
                auto string_values = string_data_();
                if (!string_values.empty())
                {
                    listener_->on_string_data_read(
                        string_values,
                        timestamp);
                }
            }
    }
}


////////////////////////////////////////////////////
// VALUES METHODS
////////////////////////////////////////////////////

void ReaderHandler::listener(const std::shared_ptr<Listener>& listener)
{
    listener_ = listener;
}

std::shared_ptr<Listener> ReaderHandler::listener() const
{
    return listener_;
}

std::string ReaderHandler::topic_name() const
{
    return topic_->get_name();
}

std::string ReaderHandler::type_name() const
{
    return topic_->get_type_name();
}


////////////////////////////////////////////////////
// AUXILIAR STATIC METHODS
////////////////////////////////////////////////////

eprosima::fastdds::dds::StatusMask ReaderHandler::default_listener_mask_()
{
    eprosima::fastdds::dds::StatusMask mask;

    // Erase all
    mask.any();
    // Only listen these callbacks (and the DomainParticipantListener ones)
    mask << eprosima::fastdds::dds::StatusMask::data_available();

    return mask;
}

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */
