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
 * @file dialogselecttopics.cpp
 */

#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFileDialog>

#include "dialogselecttopics.h"
#include "ui_dialogselecttopics.h"
#include "utils/utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

DialogSelectTopics::DialogSelectTopics(
        const Configuration& configuration,
        std::shared_ptr<fastdds::TopicDataBase> discovery_database,
        UiListener* listener,
        QWidget* parent /* = nullptr */)
    : QDialog(parent)
    , ui(new Ui::DialogSelectTopics)
    , discovery_database_(discovery_database)
    , configuration_(configuration)
    , listener_(listener)
    , domain_id_connected_(configuration.domain_id) // This is initialized here as it does not come from configuration
{
    ui->setupUi(this);

    // NOTE: This is needed as the connection by name is not working properly. It shows message:
    // QMetaObject::connectSlotsByName: No matching signal for on_reset_view()
    // when on_reset_view slot exists
    connect(
        this,
        &DialogSelectTopics::topic_discovery_signal,
        this,
        &DialogSelectTopics::on_topic_discovery_slot);

    connect(
        this,
        &DialogSelectTopics::reset_view_signal,
        this,
        &DialogSelectTopics::on_reset_view_slot);

    connect(
        this,
        &DialogSelectTopics::connection_to_domain_signal,
        this,
        &DialogSelectTopics::on_connection_to_domain_slot);

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

void DialogSelectTopics::on_lineEditFilter_textChanged(const QString& search_string)
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

    // Change domain connected
    domain_id_selected_ =
            static_cast<unsigned int>(ui->domainid_spin->value());

    // Call listener to connect to domain
    listener_->on_domain_connection(
        static_cast<unsigned int>(domain_id_selected_));
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
    add_xml_file_(utils::QString_to_string(file_path));
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
    std::vector<std::string> xml_files =
            utils::get_files_in_dir(utils::QString_to_string(dir_path), "xml", false);

    // For each file, add xml file to object
    for (const auto& file_path : xml_files)
    {
        add_xml_file_(file_path);
    }
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

void DialogSelectTopics::on_domainid_spin_valueChanged(
        int arg1)
{
    DEBUG("Calling on_domainid_spin_valueChanged");

    // Check if button of connection must be enabled
    check_domain_button_must_be_enable_();
}

void DialogSelectTopics::on_topic_discovery_slot(
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

void DialogSelectTopics::on_reset_view_slot()
{
    DEBUG("Calling on_reset_view");
    clean_topics_list_();
    reset_to_configuration_();
}

void DialogSelectTopics::on_connection_to_domain_slot(
        const uint32_t domain_id)
{
    domain_id_connected_ = domain_id;

    ui->current_domain_label->setText(
        QString::number(domain_id_connected_));

    check_domain_button_must_be_enable_();
}

void DialogSelectTopics::check_domain_button_must_be_enable_()
{
    DEBUG("Calling check_domain_button_must_be_enable_");

    // Set enable disable of the button
    ui->change_domain_button->setEnabled(
        domain_id_connected_ != ui->domainid_spin->value());
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
    domain_id_selected_ = configuration_.domain_id;

    ui->domainid_spin->setValue(domain_id_selected_);
    ui->current_domain_label->setText(
        QString::number(domain_id_connected_));
    check_domain_button_must_be_enable_();

    // Add XML files
    // First erase every data in the list
    ui->xml_files_list->clear();
    for (const auto& file : configuration_.xml_datatypes_files)
    {
        // Add every xml file in configuration
        ui->xml_files_list->addItem(file);
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
    for (int r = 0; r < ui->xml_files_list->count(); r++)
    {
        QListWidgetItem* item = ui->xml_files_list->item(r);
        // Store item
        configuration_.xml_datatypes_files.push_back(item->text());
    }
}

void DialogSelectTopics::add_xml_file_(
        const std::string& path)
{
    // Check it does not exist yet
    // TODO

    DEBUG("Adding xml file: " << path);

    // Add item to list
    const auto item = new QListWidgetItem(utils::string_to_QString(path));
    ui->xml_files_list->addItem(item);

    // Call listener with file
    listener_->on_xml_datatype_file_added(path);
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
