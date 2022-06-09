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

const char* FastDdsDataStreamer::PLUGIN_NAME_ = "Fast DDS DataStreamer PlotJuggler Plugin";

FastDdsDataStreamer::FastDdsDataStreamer()
    : running_(false)
{
    std::cout << "Create FastDdsDataStreamer" << std::endl;
}

FastDdsDataStreamer::~FastDdsDataStreamer()
{
    std::cout << "Destroy FastDdsDataStreamer" << std::endl;
}

bool FastDdsDataStreamer::start(QStringList*)
{
    std::cout << "Hello World" << std::endl;
    running_.store(true);
    return true;
}

void FastDdsDataStreamer::shutdown()
{
    std::cout << "Bye World" << std::endl;
    running_.store(false);
}

bool FastDdsDataStreamer::isRunning() const
{
    return running_.load();
}

const char* FastDdsDataStreamer::name() const
{
    return PLUGIN_NAME_;
}
