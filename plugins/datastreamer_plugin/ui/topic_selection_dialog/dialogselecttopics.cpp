// TODO add license

#include <iostream>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "dialogselecttopics.h"
#include "ui_dialogselecttopics.h"
#include "utils/utils.hpp"

namespace eprosima {
namespace plotjuggler {
namespace ui {

DialogSelectTopics::DialogSelectTopics(
        std::shared_ptr<Configuration> configuration,
        std::shared_ptr<fastdds::TopicDataBase> discovery_database,
        UiListener* listener,
        QWidget *parent /* = nullptr */)
    : QDialog(parent)
    , ui(new Ui::DialogSelectTopics)
    , discovery_database_(discovery_database)
    , configuration_(configuration)
    , listener_(listener)
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

    ////////////
    // Reset data with the current configuration
    // Remove every topic
    clean_topics_list_();

    // FastDDS configuration
    // Set current domain
    ui->domainid_spin->setValue(configuration_->domain_id_selected);
    ui->current_domain_label->setText(
        QString::number(configuration_->domain_id_connected));
    check_domain_button_must_be_enable_();

    // Add XML files
    for (auto& file : configuration_->xml_datatypes_files)
    {
        // TODO
    }

    // Array config
    ui->discard_radiobutton->setChecked(configuration_->discard_large_arrays);
    ui->clamp_radiobutton->setChecked(configuration_->use_header_stamp);
    ui->maximum_array_spin->setValue(configuration_->max_array_size);
    ui->convert_booleans_check->setChecked(configuration_->boolean_strings_to_number);
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

void DialogSelectTopics::on_lineEditFilter_editingFinished()
{
    DEBUG("Calling on_lineEditFilter_editingFinished");
}

void DialogSelectTopics::on_change_domain_button_clicked()
{
    DEBUG("Calling on_change_domain_button_clicked");

    // Change domain connected
    configuration_->domain_id_connected =
        static_cast<unsigned int>(ui->domainid_spin->value());
    ui->current_domain_label->setText(
        QString::number(configuration_->domain_id_connected));
    check_domain_button_must_be_enable_();

    // Call listener to connect to domain
    listener_->on_domain_connection(
        static_cast<unsigned int>(configuration_->domain_id_connected));
}

void DialogSelectTopics::on_include_files_button_clicked()
{
    DEBUG("Calling on_include_files_button_clicked");
}

void DialogSelectTopics::on_include_dir_button_clicked()
{
    DEBUG("Calling on_include_dir_button_clicked");
}

void DialogSelectTopics::on_buttonBox_rejected()
{
    DEBUG("Calling on_buttonBox_rejected");
}

void DialogSelectTopics::on_buttonBox_accepted()
{
    DEBUG("Calling on_buttonBox_accepted");

    /////
    // Store every value in configuration

    // Domain id
    configuration_->domain_id_selected =
        static_cast<unsigned int>(ui->domainid_spin->value());

    // Array
    configuration_->max_array_size =
        static_cast<unsigned int>(ui->maximum_array_spin->value());
    configuration_->use_header_stamp = ui->clamp_radiobutton->isChecked();
    configuration_->discard_large_arrays = ui->discard_radiobutton->isChecked();

    configuration_->boolean_strings_to_number = ui->convert_booleans_check->isChecked();

    // Get Topics
    configuration_->topics_selected.clear();
        for (int r = 0; r < ui->listRosTopics->rowCount(); r++)
    {
        QTableWidgetItem* item = ui->listRosTopics->item(r, 0);
        // Check if this is the topic name
        if (item->isSelected())
        {
            configuration_->topics_selected.push_back(item->text());
        }
    }

    // Get XML files
    // TODO
}

void DialogSelectTopics::on_domainid_spin_valueChanged(int arg1)
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
    for (int r = 0; r < ui->listRosTopics->rowCount(); r++)
    {
        QTableWidgetItem* item = ui->listRosTopics->item(r, 0);
        // Check if this is the topic name
        if (item->text() == topic_name)
        {
            // Get also type item to make it selectable
            QTableWidgetItem* type_item = ui->listRosTopics->item(r, 1);

            // If already_in_list already in list, update registered
            already_in_list = true;
            if (!type_registered)
            {
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
                type_item->setFlags(type_item->flags() & ~Qt::ItemIsSelectable);
            }
            else
            {
                item->setFlags(item->flags() | Qt::ItemIsSelectable);
                type_item->setFlags(type_item->flags() | Qt::ItemIsSelectable);
            }
            break;
        }
    }

    // Add new topic to list if it wasnt already
    if (!already_in_list)
    {
        int new_row = ui->listRosTopics->rowCount();
        ui->listRosTopics->setRowCount(new_row + 1);

        ui->listRosTopics->setItem(new_row, 0, new QTableWidgetItem(topic_name));
        ui->listRosTopics->setItem(new_row, 1, new QTableWidgetItem(type_name));

        // In case it is not registered, make it unselectable
        if (!type_registered)
        {
            auto item = ui->listRosTopics->item(new_row, 0);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            auto item_type = ui->listRosTopics->item(new_row, 1);
            item_type->setFlags(item_type->flags() & ~Qt::ItemIsSelectable);
        }
    }

    // If topic want in list yet, order it alphabetically
    if (!already_in_list)
    {
        ui->listRosTopics->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->listRosTopics->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->listRosTopics->sortByColumn(0, Qt::AscendingOrder);
    }

    DEBUG("Finishing DialogSelectTopics::on_topic_discovery " <<
        utils::QString_to_string(topic_name));
}

void DialogSelectTopics::on_reset_view_slot()
{
    DEBUG("Calling on_reset_view");
    clean_topics_list_();
}

void DialogSelectTopics::check_domain_button_must_be_enable_()
{
    DEBUG("Calling check_domain_button_must_be_enable_");

    // Set enable disable of the button
    ui->change_domain_button->setEnabled(
        configuration_->domain_id_connected != ui->domainid_spin->value());
}

void DialogSelectTopics::clean_topics_list_()
{
    DEBUG("Calling clean_topics_list_");
    ui->listRosTopics->setRowCount(0);
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
