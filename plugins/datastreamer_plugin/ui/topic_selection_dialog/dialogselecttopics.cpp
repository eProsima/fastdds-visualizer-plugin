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
 * @file dialogselecttopics.cpp
 */

#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFileDialog>

#include "dialogselecttopics.h"
#include "ui_dialogselecttopics.h"
#include "ui/domain_selection_dialog/dialogselectdomain.h"
#include "utils/utils.hpp"
#include "utils/Exception.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

DialogSelectTopics::DialogSelectTopics(
        const Configuration& configuration,
        std::shared_ptr<fastdds::TopicDataBase> discovery_database,
        UiListener* listener,
        QWidget* parent /* = nullptr */)
    : QDialog(parent)
    , ui(new Ui::DialogSelectTopics)  // This object is handled by Qt and it will be correctly deleted
    , discovery_database_(discovery_database)
    , configuration_(configuration)
    , listener_(listener)
    , domain_id_connected_(configuration.domain_id) // This is initialized here as it does not come from configuration
    , server_ip_(configuration.server_ip)
    , server_port_(configuration.server_port)
    , select_all_topics_(QKeySequence(Qt::CTRL + Qt::Key_A), this)
    , deselect_all_topics_(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A), this)
{
    ui->setupUi(this);

    // NOTE: This is needed as the connection by name is not working properly. It shows message:
    // QMetaObject::connectSlotsByName: No matching signal for on_reset_view()
    // when on_reset_view slot exists
    connect(
        this,
        &DialogSelectTopics::topic_discovery_signal,
        this,
        &DialogSelectTopics::on_topicDiscovery);

    connect(
        this,
        &DialogSelectTopics::reset_view_signal,
        this,
        &DialogSelectTopics::on_resetView);

    connect(
        this,
        &DialogSelectTopics::connection_to_domain_signal,
        this,
        &DialogSelectTopics::on_connectionToDomain);

    connect(
        this,
        &DialogSelectTopics::connection_to_server_signal,
        this,
        &DialogSelectTopics::on_connectionToServer);

    /////
    // Set shortcuts

    select_all_topics_.setContext(Qt::WindowShortcut);
    deselect_all_topics_.setContext(Qt::WindowShortcut);

    // In case selected is pressed, select all topics VISIBLE in the list
    connect(&select_all_topics_, &QShortcut::activated, ui->listDdsTopics, [this]()
            {
                for (int row = 0; row < ui->listDdsTopics->rowCount(); row++)
                {
                    if (!ui->listDdsTopics->isRowHidden(row) &&
                    !ui->listDdsTopics->item(row, TopicNameTableIndex_)->isSelected())
                    {
                        ui->listDdsTopics->selectRow(row);
                    }
                }
            });

    // In case deselected is pressed, deselect all topics in the list
    connect(&deselect_all_topics_, &QShortcut::activated, ui->listDdsTopics, &QAbstractItemView::clearSelection);


    ////////////
    // Reset data with the current configuration
    // Remove every topic
    reset_to_configuration_();

    // Set names in columns in select topics (not done in ui because Qt Designer does not support it)
    ui->listDdsTopics->setHorizontalHeaderLabels(QStringList() << tr("Topic Name") << tr("Data Type name"));

    ////////////
    // UNSUPPORTED
    ui->convert_booleans_check->setEnabled(false);
}

DialogSelectTopics::~DialogSelectTopics()
{
    delete ui;
}

void DialogSelectTopics::reset()
{
    // Emit signal to reset in view thread
    emit reset_view_signal();
}

void DialogSelectTopics::reset_to_configuration_(
        const Configuration& configuration)
{
    // Set new configuration as current and call reset
    configuration_ = configuration;
    emit reset_view_signal();
}

const Configuration& DialogSelectTopics::get_configuration() const
{
    return configuration_;
}

void DialogSelectTopics::connect_to_domain(
        const uint32_t domain_id)
{
    emit connection_to_domain_signal(domain_id);
}

void DialogSelectTopics::connect_to_server(
        const uint32_t domain_id,
        const std::string& server_ip,
        unsigned int server_port)
{
    emit connection_to_server_signal(domain_id, utils::string_to_QString(server_ip), server_port);
}

void DialogSelectTopics::on_lineEditFilter_textChanged(
        const QString& search_string)
{
    DEBUG("Calling on_lineEditFilter_editingFinished");

    // Split filter string by spaces
    QStringList spaced_items = search_string.split(' ');

    // Loop over every item
    for (int row = 0; row < ui->listDdsTopics->rowCount(); row++)
    {
        // Get item and its topic name
        const auto item = ui->listDdsTopics->item(row, TopicNameTableIndex_);
        QString topic_name = item->text();
        bool toHide = true;

        // For every filter set, check if the topic name contains the filter
        for (const auto& filter_name : spaced_items)
        {
            if (topic_name.contains(filter_name))
            {
                toHide = false;
                break;
            }
        }

        // If the topic name does not contain any of the filters, hide it
        ui->listDdsTopics->setRowHidden(row, toHide);
    }
}

void DialogSelectTopics::on_convert_booleans_check_stateChanged(
        int)
{
    // TODO
}

void DialogSelectTopics::on_change_domain_button_clicked()
{
    DEBUG("Calling on_change_domain_button_clicked");

    // Start a new dialog to select a domain
    DialogSelectDomain* change_domain_dialog = new DialogSelectDomain(domain_id_connected_, this);
    int dialog_result = change_domain_dialog->exec();

    // If the user selected a domain, connect to it
    if (dialog_result == QDialog::Accepted)
    {
        // Get the domain id
        const uint32_t domain_id = change_domain_dialog->get_selected_domain_id();
        DEBUG("New domain id selected: " << domain_id);

        // NOTE: This domain_id could not be the same as the one connected (if the dialog is correct)
        // Nevertheless lets check it
        if (domain_id != domain_id_connected_)
        {
            // Get server ip and port before the reset called by on_domain_connection
            QString server_ip = ui->server_ip->text();
            unsigned int server_port = ui->server_port->text().toUInt();

            // Call listener to connect to domain
            listener_->on_domain_connection(
                static_cast<unsigned int>(domain_id));
            
            // Set the server ip and port to the value before the reset called by on_domain_connection
            on_connectionToServer(domain_id, server_ip, server_port);
        }
    }
    // In case the dialog is cancelled, do nothing

    // Delete the dialog
    delete change_domain_dialog;
}

void DialogSelectTopics::on_use_discovery_server_button_clicked() {
    DEBUG("Calling on_use_discovery_server_button_clicked");
    // Get server ip and port
    QString server_ip = ui->server_ip->text();
    unsigned int server_port = ui->server_port->text().toUInt();

    if (ui->use_discovery_server_button->isChecked()) {
        // Call listener to connect to server
        listener_->on_server_connection(domain_id_connected_, server_ip.toStdString(), server_port);
        ui->use_discovery_server_button->setText("ON");
        ui->change_domain_button->setEnabled(false);
    } else {
        // Call listener to connect to domain
        listener_->on_domain_connection(domain_id_connected_);
        // Set the server ip and port to the value before the reset called by on_domain_connection
        on_connectionToServer(domain_id_connected_, server_ip, server_port);
        ui->use_discovery_server_button->setText("OFF");
        ui->change_domain_button->setEnabled(true);
    }
}

void DialogSelectTopics::on_include_files_button_clicked()
{
    DEBUG("Calling on_include_files_button_clicked");

    // Let user to choose the file
    QString file_path = QFileDialog::getOpenFileName(
        this,
        tr("Select .xml Data Type file"),
        QDir::homePath(),
        tr("XML files (*.xml)"));

    // Check if dialog has been cancelled
    if (file_path.isEmpty() || file_path.isNull())
    {
        return;
    }

    // Add it to this object
    add_xml_file_(file_path);
}

void DialogSelectTopics::on_include_dir_button_clicked()
{
    DEBUG("Calling on_include_dir_button_clicked");

    // Let user choose the directory from dialog
    QString dir_path = QFileDialog::getExistingDirectory(
        this, tr("Select a Directory"), QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Check if dialog has been cancelled
    if (dir_path.isEmpty() || dir_path.isNull())
    {
        return;
    }

    // Loop the directory searching for xml files
    QStringList xml_files =
            utils::get_files_in_dir(dir_path, "xml", false);

    // For each file, add xml file to object
    for (const auto& file_path : xml_files)
    {
        add_xml_file_(file_path);
    }
}

void DialogSelectTopics::on_clean_xml_files_button_clicked()
{
    ui->xml_files_list->clear();
}

void DialogSelectTopics::on_buttonBox_rejected()
{
    DEBUG("Calling on_buttonBox_rejected");
}

void DialogSelectTopics::on_buttonBox_accepted()
{
    DEBUG("Calling on_buttonBox_accepted");
    update_configuration_();
}

void DialogSelectTopics::on_topicDiscovery(
        const QString& topic_name,
        const QString& type_name,
        bool type_registered)
{
    DEBUG("Calling DialogSelectTopics::on_topic_discovery " <<
            utils::QString_to_string(topic_name) << " " <<
            utils::QString_to_string(type_name) << " " <<
            type_registered);

    // Check if topic already exist
    bool already_in_list = false;
    // Look for every item in list
    for (int r = 0; r < ui->listDdsTopics->rowCount(); r++)
    {
        QTableWidgetItem* item = ui->listDdsTopics->item(r, 0);
        // Check if this is the topic name
        if (item->text() == topic_name)
        {
            already_in_list = true;
            type_format_(r, type_registered);
        }
    }

    // Add new topic to list if it wasnt already
    if (!already_in_list)
    {
        int new_row = ui->listDdsTopics->rowCount();
        ui->listDdsTopics->setRowCount(new_row + 1);

        ui->listDdsTopics->setItem(new_row, TopicNameTableIndex_, new QTableWidgetItem(topic_name));
        ui->listDdsTopics->setItem(new_row, TypeNameTableIndex_, new QTableWidgetItem(type_name));
        DEBUG("set item " << new_row << " 1");

        // In case it is not registered, make it unselectable
        type_format_(new_row, type_registered);
    }

    // If topic want in list yet, order it alphabetically
    if (!already_in_list)
    {
        ui->listDdsTopics->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->listDdsTopics->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

        // TODO: this line causes a segfault when sorting topics, as the item with type dissapear
        // and when trying to acces it to change the selectable, it breaks with seg fault
        // ui->listDdsTopics->sortByColumn(0, Qt::AscendingOrder);
    }

    DEBUG("Finishing DialogSelectTopics::on_topic_discovery " <<
            utils::QString_to_string(topic_name));
}

void DialogSelectTopics::on_resetView()
{
    DEBUG("Calling on_reset_view");
    clean_topics_list_();
    reset_to_configuration_();
}

void DialogSelectTopics::on_connectionToDomain(
        const uint32_t domain_id)
{
    domain_id_connected_ = domain_id;

    ui->current_domain_label->setText(
        QString::number(domain_id_connected_));
}

void DialogSelectTopics::on_connectionToServer(
        const uint32_t domain_id,
        const QString& server_ip,
        unsigned int server_port)
{
    domain_id_connected_ = domain_id;
    server_ip_ = server_ip.toStdString();
    server_port_ = server_port;

    ui->current_domain_label->setText(
        QString::number(domain_id_connected_));
    ui->server_ip->setText(server_ip_.c_str());
    ui->server_port->setText(QString::number(server_port_));
}

void DialogSelectTopics::clean_topics_list_()
{
    DEBUG("Calling clean_topics_list_");
    ui->listDdsTopics->setRowCount(0);
}

void DialogSelectTopics::reset_to_configuration_()
{
    ////////////
    // Reset data with the current configuration
    // Remove every topic
    clean_topics_list_();

    // FastDDS configuration
    // Set current domain
    on_connectionToDomain(configuration_.domain_id);
    on_connectionToServer(configuration_.domain_id, utils::string_to_QString(configuration_.server_ip), configuration_.server_port);

    // Add XML files
    // First erase every data in the list
    for (const auto& file : configuration_.xml_datatypes_files)
    {
        // Add every xml file in configuration
        ui->xml_files_list->addItem(file);
    }

    // Add every xml file stored already in configuration
    ui->xml_files_list->clear();
    for (const auto& xml_file : configuration_.xml_datatypes_files)
    {
        try
        {
            add_xml_file_(xml_file);
        }
        catch (const IncorrectParamException& e)
        {
            // Error adding xml file, ignoring this file and it will not be added to the list
            WARNING("Error loading xml file " << utils::QString_to_string(xml_file));
        }
    }

    // Array config
    ui->discard_radiobutton->setChecked(configuration_.data_type_configuration.discard_large_arrays);
    ui->clamp_radiobutton->setChecked(configuration_.data_type_configuration.use_header_stamp);
    ui->maximum_array_spin->setValue(configuration_.data_type_configuration.max_array_size);
    ui->convert_booleans_check->setChecked(configuration_.boolean_strings_to_number);
}

void DialogSelectTopics::update_configuration_()
{
    // Store every value in configuration

    // Domain id
    configuration_.domain_id =
            static_cast<unsigned int>(domain_id_connected_);
    configuration_.server_ip = server_ip_;
    configuration_.server_port = server_port_;

    // Array
    configuration_.data_type_configuration.max_array_size =
            static_cast<unsigned int>(ui->maximum_array_spin->value());
    configuration_.data_type_configuration.use_header_stamp = ui->clamp_radiobutton->isChecked();
    configuration_.data_type_configuration.discard_large_arrays = ui->discard_radiobutton->isChecked();

    configuration_.boolean_strings_to_number = ui->convert_booleans_check->isChecked();

    // Get Topics
    configuration_.topics_selected.clear();
    for (int r = 0; r < ui->listDdsTopics->rowCount(); r++)
    {
        QTableWidgetItem* item = ui->listDdsTopics->item(r, TopicNameTableIndex_);
        // Check if this is the topic name
        if (item->isSelected())
        {
            configuration_.topics_selected.push_back(item->text());
        }
    }

    // Get XML files
    configuration_.xml_datatypes_files.clear();
    for (int r = 0; r < ui->xml_files_list->count(); r++)
    {
        QListWidgetItem* item = ui->xml_files_list->item(r);
        // Store item
        configuration_.xml_datatypes_files.push_back(item->text());
    }
}

void DialogSelectTopics::add_xml_file_(
        const QString& path)
{
    // Check it does not exist yet
    // TODO

    DEBUG("Adding xml file: " << utils::QString_to_string(path));

    // Call listener with file
    listener_->on_xml_datatype_file_added(utils::QString_to_string(path));

    // Add item to list
    // NOTE: add it to list after listener call, so if it fails it is not added
    const auto item = new QListWidgetItem(path);
    ui->xml_files_list->addItem(item);
}

void DialogSelectTopics::type_format_(
        const int row,
        bool registered)
{
    DEBUG("Changing item format in row " << row);
    if (registered)
    {
        registered_type_change_format_(row);
    }
    else
    {
        unregistered_type_change_format_(row);
    }
}

void DialogSelectTopics::registered_type_change_format_(
        const int row )
{
    // Take items
    QTableWidgetItem* item = ui->listDdsTopics->item(row, TopicNameTableIndex_);
    QTableWidgetItem* type_item = ui->listDdsTopics->item(row, TypeNameTableIndex_);

    // Set them as selectable
    item->setFlags(item->flags() | Qt::ItemIsSelectable);
    type_item->setFlags(item->flags() | Qt::ItemIsSelectable);

    // Change color to black
    item->setForeground(Qt::black);
    type_item->setForeground(Qt::black);
}

void DialogSelectTopics::unregistered_type_change_format_(
        const int row)
{
    // Take items
    QTableWidgetItem* item = ui->listDdsTopics->item(row, TopicNameTableIndex_);
    QTableWidgetItem* type_item = ui->listDdsTopics->item(row, TypeNameTableIndex_);

    // Set them as not selectable
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    type_item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    // Change color to grey
    item->setForeground(Qt::gray);
    type_item->setForeground(Qt::gray);
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
