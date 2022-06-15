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
    connect(
        this,
        &DialogSelectTopics::topic_discovery_signal,
        this,
        &DialogSelectTopics::on_topic_discovery_slot);
}

DialogSelectTopics::~DialogSelectTopics()
{
    delete ui;
}

void DialogSelectTopics::on_lineEditFilter_editingFinished()
{
    DEBUG("Calling on_lineEditFilter_editingFinished");
}
void DialogSelectTopics::on_convert_booleans_check_stateChanged(int arg1)
{
    DEBUG("Calling on_convert_booleans_check_stateChanged");
}

void DialogSelectTopics::on_change_domain_button_clicked()
{
    DEBUG("Calling on_change_domain_button_clicked");
    // Call listener to connect to domain
    listener_->on_domain_connection(
        static_cast<unsigned int>(ui->domainid_spin->value()));
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
}

void DialogSelectTopics::on_domainid_spin_valueChanged(int arg1)
{
    DEBUG("Calling on_domainid_spin_valueChanged");
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
            // If already_in_list already in list, update registered
            already_in_list = true;
            if (!type_registered)
            {
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
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
        }

        // Always make type unselectable
        auto item = ui->listRosTopics->item(new_row, 1);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    // If topic want in list yet, order it alphabetically
    if (!already_in_list)
    {
        ui->listRosTopics->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->listRosTopics->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->listRosTopics->sortByColumn(0, Qt::AscendingOrder);
    }
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
