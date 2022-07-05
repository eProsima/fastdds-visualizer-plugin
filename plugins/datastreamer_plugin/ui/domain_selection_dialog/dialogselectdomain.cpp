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
 * @file dialogselectdomain.cpp
 */

#include <iostream>
#include <QDialogButtonBox>
#include <QPushButton>

#include "dialogselectdomain.h"
#include "ui_dialogselectdomain.h"

namespace eprosima {
namespace plotjuggler {
namespace ui {

DialogSelectDomain::DialogSelectDomain(
        const uint32_t current_domain_id,
        QWidget* parent /* = nullptr */)
    : QDialog(parent)
    , ui(new Ui::DialogSelectDomain)
    , current_domain_id_(current_domain_id)
    , selected_domain_id_(current_domain_id)
{
    ui->setupUi(this);

    // Set the current domain id in label.
    ui->domainIdLabel->setText(QString::number(current_domain_id));

    check_accept_enable_();
}

uint32_t DialogSelectDomain::get_selected_domain_id() const
{
    return selected_domain_id_;
}

void DialogSelectDomain::on_buttonBox_rejected()
{
    // Do nothing
}

void DialogSelectDomain::on_buttonBox_accepted()
{
    selected_domain_id_ = ui->domainSpinBox->value();
}

void DialogSelectDomain::on_domainSpinBox_valueChanged(
        int arg1)
{
    check_accept_enable_();
}

void DialogSelectDomain::check_accept_enable_()
{
    // Only make accept button enabled if domain is different from current domain
    if (ui->domainSpinBox->value() != current_domain_id_)
    {
        ui->buttonBoxDomainDialog->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        ui->buttonBoxDomainDialog->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */
