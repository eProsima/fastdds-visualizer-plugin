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

/**
 * @brief TODO
 */
class FastDdsDataStreamer : public PJ::DataStreamer
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

    bool start(QStringList*) override;

    void shutdown() override;

    bool isRunning() const override;

    const char* name() const override;

protected:

    std::atomic<bool> running_;

    static const char* PLUGIN_NAME_;
};

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_DATASTREAMER_DATASTREAMER_HPP_
