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
 * @file FastDdsDataStreamer.cpp
 */

#include "FastDdsDataStreamer.hpp"
#include "ui/topic_selection_dialog/dialogselecttopics.h"
#include "utils/utils.hpp"
#include "utils/Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace datastreamer {

FastDdsDataStreamer::FastDdsDataStreamer()
    : running_(false)
    , configuration_(QString(CONFIGURATION_SETTINGS_PREFIX_))
    , fastdds_handler_(this)
    , select_topics_dialog_(
        configuration_,
        fastdds_handler_.get_topic_data_base(),
        this)
{
    DEBUG("Create FastDdsDataStreamer");
}

FastDdsDataStreamer::~FastDdsDataStreamer()
{
    DEBUG("Destroy FastDdsDataStreamer");
    shutdown();
}

bool FastDdsDataStreamer::start(
        QStringList*)
{
    DEBUG("FastDdsDataStreamer::start");

    // Check if it is already running
    if (running_)
    {
        return true;
    }

    // Creating a default DomainParticipant in domain by default (configuration_)
    this->connect_to_domain_(configuration_.domain_id);

    // Reset dialogselecttopic to current configuration
    // NOTE: If this is done before connecting to domain, xml files will not be added
    select_topics_dialog_.reset_to_configuration_(configuration_);

    // Execute Dialog
    int dialog_result = select_topics_dialog_.exec();

    // Check if Accept has been pressed
    if (dialog_result != QDialog::Accepted)
    {
        DEBUG("Dialog closed cancelled, exiting");
        return false;
    }

    // Get configuration from dialog
    configuration_ = select_topics_dialog_.get_configuration();
    // Store as default configuration
    configuration_.save_default_settings(CONFIGURATION_SETTINGS_PREFIX_);

    // Topics selected
    const auto& topics = configuration_.topics_selected;  // Decorator variable to avoid calling internal member

    if (topics.empty())
    {
        DEBUG("No topics selected, exiting");
        throw InitializationException("No topics selected.");
    }

    for (const auto& topic : topics)
    {
        // Create a subscription
        fastdds_handler_.create_subscription(
            utils::QString_to_string(topic),
            configuration_.data_type_configuration);
    }

    // Get all series from topics and create them
    dataMap().clear();
    create_series_();

    running_ = true;
    return true;
}

void FastDdsDataStreamer::shutdown()
{
    DEBUG("Bye World");

    // If it is running, stop it
    if (running_)
    {
        running_ = false;

        // Reset FastDDS so DDS entities are destroyed
        fastdds_handler_.reset();
        select_topics_dialog_.reset();
    }
}

bool FastDdsDataStreamer::isRunning() const
{
    return running_;
}

const char* FastDdsDataStreamer::name() const
{
    return PLUGIN_NAME_;
}

bool FastDdsDataStreamer::xmlSaveState(
        QDomDocument& doc,
        QDomElement& plugin_elem) const
{
    return configuration_.xmlSaveState(doc, plugin_elem);
}

bool FastDdsDataStreamer::xmlLoadState(
        const QDomElement& parent_element)
{
    return configuration_.xmlLoadState(parent_element);
}

////////////////////////////////////////////////////
// FASTDDS LISTENER METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::on_data_available()
{
    DEBUG("FastDdsDataStreamer on_data_available");

    // Locking DataStream
    std::lock_guard<std::mutex> lock(mutex());

    // Create series from new received sample
    create_series_();
}

void FastDdsDataStreamer::on_double_data_read(
        const std::vector<std::pair<std::string, double>>& data_per_topic_value,
        double timestamp)
{
    DEBUG("FastDdsDataStreamer on_double_data_read");

    // Locking DataStream
    std::lock_guard<std::mutex> lock(mutex());

    for (const auto& data : data_per_topic_value)
    {
        DEBUG("Adding to numeric series " << data.first << " value " << data.second << " with timestamp " << timestamp);
        if (dataMap().numeric.find(data.first) == dataMap().numeric.end())
        {
            throw InconsistencyException("Series " + data.first + " not found.");
        }
        // Get data map
        auto& series = dataMap().numeric.find(data.first)->second;

        // Add data to series
        series.pushBack( { timestamp, data.second});
        DEBUG("...Data added to series");
    }

    emit dataReceived();
}

void FastDdsDataStreamer::on_string_data_read(
        const std::vector<std::pair<std::string, std::string>>& data_per_topic_value,
        double timestamp    )
{
    DEBUG("FastDdsDataStreamer on_string_data_read");

    // Locking DataStream
    std::lock_guard<std::mutex> lock(mutex());

    for (const auto& data : data_per_topic_value)
    {
        DEBUG("Adding to string series " << data.first << " value " << data.second << " with timestamp " << timestamp);

        // Get data map
        auto& series = dataMap().strings.find(data.first)->second;
        // Add data to series
        series.pushBack( { timestamp, data.second});
    }

    emit dataReceived();
}

void FastDdsDataStreamer::on_topic_discovery(
        const std::string& topic_name,
        const std::string& type_name)
{
    DEBUG("FastDdsDataStreamer topic_discovery_signal " << topic_name);
    bool type_info_available = fastdds_handler_.get_topic_data_base()->operator [](topic_name).second;

    // __FLAG__
    DEBUG("topic discovery signal with type name " << type_name << " and type info available " << type_info_available);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Emit signal to UI so it is handled from Qt thread
    emit select_topics_dialog_.topic_discovery_signal(
        utils::string_to_QString(topic_name),
        utils::string_to_QString(type_name),
        type_info_available);
}

////////////////////////////////////////////////////
// UI LISTENER METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::on_xml_datatype_file_added(
        const std::string& file_path)
{
    DEBUG("FastDdsDataStreamer on_xml_datatype_file_added " << file_path);
    fastdds_handler_.register_type_from_xml(file_path);
}

void FastDdsDataStreamer::on_domain_connection(
        unsigned int domain_id)
{
    DEBUG("FastDdsDataStreamer on_domain_connection " << domain_id);
    connect_to_domain_(domain_id);
}

void FastDdsDataStreamer::on_server_connection(
        unsigned int domain_id, const std::string& server_ip, unsigned int server_port)
{
    DEBUG("FastDdsDataStreamer on_domain_connection " << domain_id);
    connect_to_server_(domain_id, server_ip, server_port);
}

////////////////////////////////////////////////////
// AUXILIAR METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::connect_to_domain_(
        unsigned int domain_id)
{
    DEBUG("FastDdsDataStreamer connect_to_domain_ " << domain_id);

    // Reset view and handler
    select_topics_dialog_.reset();
    fastdds_handler_.reset();

    // Connect to domain
    fastdds_handler_.connect_to_domain(domain_id);
    select_topics_dialog_.connect_to_domain(domain_id);
}

void FastDdsDataStreamer::connect_to_server_(
        unsigned int domain_id, const std::string& server_ip, unsigned int server_port)
{
    DEBUG("FastDdsDataStreamer connect_to_server_ " << server_ip<<":"<<server_port<<" using domain "<<domain_id);
    // Reset view and handler
    select_topics_dialog_.reset();
    fastdds_handler_.reset();

    // Connect to domain
    fastdds_handler_.connect_to_server(domain_id, server_ip, server_port);
    select_topics_dialog_.connect_to_server(domain_id, server_ip, server_port);
}

void FastDdsDataStreamer::create_series_()
{
    // Get all series from topics and create them
    // NUMERIC
    std::vector<types::DatumLabel> numeric_series = fastdds_handler_.numeric_data_series_names();
    for (const auto& series : numeric_series)
    {
        // Create a series
        DEBUG("Creating numeric series: " << series);
        dataMap().addNumeric(series);
    }

    // STRING
    std::vector<types::DatumLabel> string_series = fastdds_handler_.string_data_series_names();
    for (const auto& series : string_series)
    {
        // Create a series
        DEBUG("Creating string series: " << series);
        dataMap().addStringSeries(series);
    }
}

} /* namespace datastreamer */
} /* namespace plotjuggler */
} /* namespace eprosima */
