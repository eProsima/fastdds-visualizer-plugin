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

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_READERHANDLER_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_READERHANDLER_HPP_

#include <atomic>

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>

#include "FastDdsListener.hpp"
#include "utils/dynamic_types_utils.hpp"
#include "utils/DataTypeConfiguration.hpp"

namespace eprosima {
namespace plotjuggler {
namespace fastdds {

/**
 * @brief TODO
 *
 */
struct ReaderHandler : public eprosima::fastdds::dds::DataReaderListener
{
public:

    ////////////////////////////////////////////////////
    // CREATION & DESTRUCTION
    ////////////////////////////////////////////////////

    ReaderHandler(
            eprosima::fastdds::dds::Topic* topic,
            eprosima::fastdds::dds::DataReader* datareader,
            eprosima::fastrtps::types::DynamicType_ptr type,
            FastDdsListener* listener,
            const DataTypeConfiguration& data_type_configuration);


    virtual ~ReaderHandler();

    ReaderHandler& operator =(
            ReaderHandler&& other);

    ReaderHandler& operator =(
            const ReaderHandler& other);

    ////////////////////////////////////////////////////
    // INTERACTION METHODS
    ////////////////////////////////////////////////////

    void stop();


    ////////////////////////////////////////////////////
    // LISTENER [ DATAREADER ] METHODS
    ////////////////////////////////////////////////////

    void on_data_available(
            eprosima::fastdds::dds::DataReader* reader) override;


    ////////////////////////////////////////////////////
    // VALUES METHODS
    ////////////////////////////////////////////////////

    const std::string& topic_name() const;

    const std::string& type_name() const;

    std::vector<types::DatumLabel> numeric_data_series_names() const;

    std::vector<types::DatumLabel> string_data_series_names() const;


    ////////////////////////////////////////////////////
    // AUXILIAR STATIC METHODS
    ////////////////////////////////////////////////////

    /**
     * @brief Get default mask
     *
     * Callbacks accepted by this mask:
     * - on_data_available
     *
     * @return eprosima::fastdds::dds::StatusMask with callbacks needed
     */
    static eprosima::fastdds::dds::StatusMask default_listener_mask_();


    ////////////////////////////////////////////////////
    // INTERNAL VARIABLES
    ////////////////////////////////////////////////////

    FastDdsListener* listener_;


    ////////////////////////////////////////////////////
    // FAST DDS POINTERS
    ////////////////////////////////////////////////////

    //! Topic related with this DataReader
    eprosima::fastdds::dds::Topic* topic_;

    //! DataReader
    eprosima::fastdds::dds::DataReader* reader_;

    //! Type Informantion
    eprosima::fastrtps::types::DynamicType_ptr type_;

    //! Data Type element
    eprosima::fastrtps::types::DynamicData_ptr data_;

    std::atomic<bool> stop_;

    utils::TypeIntrospectionCollection numeric_data_info_;
    utils::TypeIntrospectionCollection string_data_info_;

    utils::TypeIntrospectionNumericStruct numeric_data_;
    utils::TypeIntrospectionStringStruct string_data_;

};

} /* namespace fastdds */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_FASTDDS_READERHANDLER_HPP_
