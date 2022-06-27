// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Exception.hpp
 */

#ifndef _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_EXCEPTION_HPP_
#define _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

namespace eprosima {
namespace plotjuggler {

/**
 * @brief Base class for all exceptions thrown by the eProsima PlotJuggler FastDDS plugin.
 *
 * @note This class extends the std::runtime_error class to inherit functionality to be created with a custom message.
 */
class Exception : public std::runtime_error
{
public:

    using std::runtime_error::runtime_error;
};

class UnsupportedException : public Exception
{
public:

    using Exception::Exception;
};

class InitializationException : public Exception
{
public:

    using Exception::Exception;
};

class InconsistencyException : public Exception
{
public:

    using Exception::Exception;
};

class IncorrectParamException : public Exception
{
public:

    using Exception::Exception;
};

} // namespace ddsrouter
} // namespace eprosima

#endif // _EPROSIMA_PLOTJUGGLERFASTDDSPLUGIN_PLUGINS_DATASTREAMERPLUGIN_UTILS_EXCEPTION_HPP_

