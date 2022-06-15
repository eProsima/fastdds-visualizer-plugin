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
 * @file FastDdsDataStreamer.cpp
 */

#include "FastDdsDataStreamer.hpp"
#include "ui/topic_selection_dialog/dialogselecttopics.h"

namespace eprosima {
namespace plotjuggler {
namespace datastreamer {

const char* FastDdsDataStreamer::PLUGIN_NAME_ = "Fast DDS DataStreamer PlotJuggler Plugin";

FastDdsDataStreamer::FastDdsDataStreamer()
    : running_(false)
    , fastdds_handler_(std::make_unique<fastdds::Handler>(this))
    , configuration_(std::make_shared<ui::Configuration>())
    , select_topics_dialog_(std::make_unique<ui::DialogSelectTopics>(
        configuration_,
        fastdds_handler_->get_topic_data_base(),
        this))
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
    if (running_.load())
    {
        return true;
    }

    // Reset FastDDS and UI
    fastdds_handler_->reset();
    select_topics_dialog_->reset();

    // Creating a default DomainParticipant in domain by default (configuration_)
    fastdds_handler_->connect_to_domain(configuration_->domain_id);

    // Execute Dialog
    int dialog_result = select_topics_dialog_->exec();

    // Check if Accept has been pressed
    if (dialog_result != QDialog::Accepted)
    {
        DEBUG("Dialog closed cancelled, exiting");
        return false;
    }
    else
    {
        // TODO add check that any topic is added
        DEBUG("Dialog closed accepted, creating subscriptions");
    }

    running_.store(true);
    return true;
}

void FastDdsDataStreamer::shutdown()
{
    DEBUG("Bye World");

    // If it is running, stop it
    if (running_.load())
    {
        running_.store(false);

        // Reset FastDDS so DDS entities are destroyed
        fastdds_handler_->reset();
        select_topics_dialog_->reset();
    }
}

bool FastDdsDataStreamer::isRunning() const
{
    return running_.load();
}

const char* FastDdsDataStreamer::name() const
{
    return PLUGIN_NAME_;
}


////////////////////////////////////////////////////
// FASTDDS LISTENER METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::on_topic_discovery(
        const std::string& topic_name,
        const std::string& type_name,
        bool type_registered)
{
    DEBUG("FastDdsDataStreamer topic_discovery_signal " << topic_name);
    emit select_topics_dialog_->topic_discovery_signal(
        utils::string_to_QString(topic_name),
        utils::string_to_QString(type_name),
        type_registered);
}


////////////////////////////////////////////////////
// UI LISTENER METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::on_domain_connection(
        unsigned int domain_id)
{
    DEBUG("FastDdsDataStreamer on_domain_connection " << domain_id);
    fastdds_handler_->connect_to_domain(configuration_->domain_id);
}


////////////////////////////////////////////////////
// AUXILIAR METHODS
////////////////////////////////////////////////////

void FastDdsDataStreamer::connect_to_domain_(
        unsigned int domain_id)
{
    DEBUG("FastDdsDataStreamer connect_to_domain_ " << domain_id);
    fastdds_handler_->connect_to_domain(domain_id);
}

} /* namespace datastreamer */
} /* namespace plotjuggler */
} /* namespace eprosima */
