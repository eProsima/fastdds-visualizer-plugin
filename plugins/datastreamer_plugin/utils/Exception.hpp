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

#include <exception>
#include <string>

namespace eprosima {
namespace plotjuggler {

/**
 * @brief Base class for all exceptions thrown by the eProsima PlotJuggler FastDDS plugin.
 *
 * This class extends the std::exception class and adds the functionality to raise an exception with a custom message.
 *
 * @note This class is implemented in header because of its simplicity in order to save time.
 */
class Exception : public std::exception
{

public:

    /**
     * @brief Construct a new ddsrouter::Exception object
     *
     * @param message The message to be returned by what()
     */
    Exception(
            const char* message) noexcept
        : message_(message)
    {
    }

    /**
     * @brief Construct a new ddsrouter::Exception object
     *
     * @param message The message to be returned by what()
     */
    Exception(
            const std::string& message) noexcept
        : message_(message)
    {
    }

    /**
     * @brief Copies the ddsrouter::Exception object into a new one
     *
     * @param other The original exception object to copy
     */
    Exception(
            const Exception& other) = default;

    /**
     * @brief Copies the ddsrouter::Exception object into the current one
     *
     * @param other The original exception object to copy
     * @return the current ddsrouter::Exception object after the copy
     */
    Exception& operator =(
            const Exception& other) = default;

    /**
     * @brief Returns the explanatory string of the exception
     *
     * @return Null-terminated string with the explanatory information
     */
    virtual const char* what() const noexcept override
    {
        return message_.c_str();
    }

protected:

    std::string message_;
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

