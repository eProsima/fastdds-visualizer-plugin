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
 * @file FastDdsDataStreamer.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_DATASTREAMER_DATASTREAMER_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_DATASTREAMER_DATASTREAMER_HPP_

#include <QtPlugin>

#include <PlotJuggler/datastreamer_base.h>

#include "ui/topic_selection_dialog/dialogselecttopics.h"
#include "ui/topic_selection_dialog/Configuration.hpp"
#include "ui/topic_selection_dialog/UiListener.hpp"
#include "fastdds/Handler.hpp"
#include "fastdds/FastDdsListener.hpp"

namespace eprosima {
namespace plotjuggler {
namespace datastreamer {

/**
 * @brief TODO
 */
class FastDdsDataStreamer :
    public PJ::DataStreamer,
    public fastdds::FastDdsListener,
    public ui::UiListener
{
    //! Macros for Qt Plugin and Object
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.FastDDSDataStreamer")
    Q_INTERFACES(PJ::DataStreamer)

public:

    /**
     * @brief Construct a new Fast Dds Data Streamer object
     *
     * @note This object is constructed at the beginning of the execution of the program,
     * even before the plugin is selected and started.
     */
    FastDdsDataStreamer();

    /**
     * @brief Construct a new Fast Dds Data Streamer object
     *
     * @note This occurs once at the end of the process when the UI closes
     */
    ~FastDdsDataStreamer();


    ////////////////////////////////////////////////////
    // DATASTREAMER PLUGIN METHODS
    ////////////////////////////////////////////////////

    bool start(
            QStringList*) override;

    void shutdown() override;

    bool isRunning() const override;

    const char* name() const override;

    // TODO: add slots to receive discovery information and user data

    ////////////////////////////////////////////////////
    // FASTDDS LISTENER METHODS
    ////////////////////////////////////////////////////

    virtual void on_double_data_read(
        const std::vector<std::pair<std::string, double>>& data_per_topic_value,
        double timestamp) override;

    virtual void on_string_data_read(
        const std::vector<std::pair<std::string, std::string>>& data_per_topic_value,
        double timestamp) override;

    virtual void on_topic_discovery(
            const std::string& topic_name,
            const std::string& type_name,
            bool type_registered) override;


    ////////////////////////////////////////////////////
    // UI LISTENER METHODS
    ////////////////////////////////////////////////////

    virtual void on_domain_connection(
        unsigned int domain_id) override;


protected:

    ////////////////////////////////////////////////////
    // AUXILIAR METHODS
    ////////////////////////////////////////////////////

    void connect_to_domain_(
        unsigned int domain_id);


    ////////////////////////////////////////////////////
    // INTERNAL VALUES
    ////////////////////////////////////////////////////

    std::shared_ptr<ui::Configuration> configuration_;

    std::unique_ptr<fastdds::Handler> fastdds_handler_;

    std::unique_ptr<ui::DialogSelectTopics> select_topics_dialog_;

    std::atomic<bool> running_;

    static const char* PLUGIN_NAME_;
};

} /* namespace datastreamer */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_DATASTREAMER_DATASTREAMER_HPP_
