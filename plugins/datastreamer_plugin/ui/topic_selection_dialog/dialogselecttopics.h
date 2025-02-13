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
 * @file dialogselecttopics.hpp
 */

#ifndef DIALOGSELECTTOPICS_H
#define DIALOGSELECTTOPICS_H

#include <memory>

#include <QDialog>
#include <QShortcut>

#include "Configuration.hpp"
#include "UiListener.hpp"
#include "fastdds/TopicDataBase.hpp"

namespace Ui {
class DialogSelectTopics;
} // namespace Ui


namespace eprosima {
namespace plotjuggler {
namespace ui {

class DialogSelectTopics : public QDialog
{
    Q_OBJECT

public:

    DialogSelectTopics(
            const Configuration& configuration,
            std::shared_ptr<fastdds::TopicDataBase> discovery_database,
            UiListener* listener,
            QWidget* parent = nullptr);

    ~DialogSelectTopics();

    void reset();

    void reset_to_configuration_(
            const Configuration& configuration);

    const Configuration& get_configuration() const;

    void connect_to_domain(
            const uint32_t domain_id);

    void connect_to_server(
                const uint32_t domain_id,
                const std::string& server_ip,
                unsigned int server_port);
signals:

    void reset_view_signal();

    void topic_discovery_signal(
            const QString& topic_name,
            const QString& type_name,
            bool type_registered);

    void connection_to_domain_signal(
            const uint32_t domain_id);

   void connection_to_server_signal(
        const uint32_t domain_id,
        const QString& server_ip,
        const uint32_t server_port);

private slots:

    void on_lineEditFilter_textChanged(
            const QString& search_string);

    void on_convert_booleans_check_stateChanged(
            int arg1);

    void on_change_domain_button_clicked();

    void on_use_discovery_server_button_clicked();

    void on_include_files_button_clicked();

    void on_include_dir_button_clicked();

    void on_clean_xml_files_button_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_resetView();

    void on_topicDiscovery(
            const QString& topic_name,
            const QString& type_name,
            bool type_registered);

    void on_connectionToDomain(
            const uint32_t domain_id);

    void on_connectionToServer(
            const uint32_t domain_id,
            const QString& server_ip,
            unsigned int server_port);

protected:

    void clean_topics_list_();

    void reset_to_configuration_();

    void update_configuration_();

    void add_xml_file_(
            const QString& path);

    void type_format_(
            const int row,
            bool registered);

    /**
     * @brief Change format of the row \c row in \c listDdsTopics to set as a registered data type
     *
     * This set the text color to black and make item selectable
     */
    void registered_type_change_format_(
            const int row);

    /**
     * @brief Change format of the row \c row in \c listDdsTopics to set as a not registered data type
     *
     * This set the text color to grey and make item non selectable
     */
    void unregistered_type_change_format_(
            const int row);

    std::shared_ptr<fastdds::TopicDataBase> discovery_database_;

    Configuration configuration_;

    UiListener* listener_;

    Ui::DialogSelectTopics* ui;

    constexpr static const unsigned int TopicNameTableIndex_ = 0;
    constexpr static const unsigned int TypeNameTableIndex_ = 1;

    unsigned int domain_id_connected_;
    //! Variables storing discovery server information
    std::string server_ip_;
    unsigned int server_port_;

    QShortcut select_all_topics_;
    QShortcut deselect_all_topics_;
};

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // DIALOGSELECTTOPICS_H
