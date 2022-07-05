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
 * @file dialogselectDomain.hpp
 */

#ifndef DIALOGSELECTDOMAIN_H
#define DIALOGSELECTDOMAIN_H

#include <memory>

#include <QDialog>
#include <QShortcut>

namespace Ui {
class DialogSelectDomain;
} // namespace Ui


namespace eprosima {
namespace plotjuggler {
namespace ui {

class DialogSelectDomain : public QDialog
{
    Q_OBJECT

public:

    DialogSelectDomain(
            const uint32_t current_domain_id,
            QWidget* parent = nullptr);

    uint32_t get_selected_domain_id() const;

private slots:

    void on_domainSpinBox_valueChanged(
            int arg1);

    void on_buttonBoxDomainDialog_accepted();

    void on_buttonBoxDomainDialog_rejected();

protected:

    void check_accept_enable_();

    const uint32_t current_domain_id_;
    uint32_t selected_domain_id_;

    Ui::DialogSelectDomain* ui;
};

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // DIALOGSELECTDOMAIN_H
