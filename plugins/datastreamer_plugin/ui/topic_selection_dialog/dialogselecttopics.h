// TODO license

#ifndef DIALOGSELECTTOPICS_H
#define DIALOGSELECTTOPICS_H

#include <memory>

#include <QDialog>

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
        std::shared_ptr<Configuration> configuration,
        std::shared_ptr<fastdds::TopicDataBase> discovery_database,
        UiListener* listener,
        QWidget *parent = nullptr);

    ~DialogSelectTopics();

    void reset();

signals:

    void reset_view_signal();

    void topic_discovery_signal(
        const QString& topic_name,
        const QString& type_name,
        bool type_registered);

private slots:

    void on_lineEditFilter_editingFinished();

    void on_convert_booleans_check_stateChanged(int arg1);

    void on_change_domain_button_clicked();

    void on_include_files_button_clicked();

    void on_include_dir_button_clicked();

    void on_domainid_spin_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_topic_discovery_slot(
        const QString& topic_name,
        const QString& type_name,
        bool type_registered);

    void on_reset_view_slot();

protected:

    void check_domain_button_must_be_enable_();

    void clean_topics_list_();

    std::shared_ptr<fastdds::TopicDataBase> discovery_database_;

    std::shared_ptr<Configuration> configuration_;

    UiListener* listener_;

    Ui::DialogSelectTopics *ui;
};

} /* namespace ui */
} /* namespace plotjuggler */
} /* namespace eprosima */

#endif // DIALOGSELECTTOPICS_H
