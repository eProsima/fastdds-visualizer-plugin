#ifndef DIALOGSELECTTOPICS_H
#define DIALOGSELECTTOPICS_H

#include <QDialog>

namespace Ui {
class DialogSelectTopics;
}

class DialogSelectTopics : public QDialog
{
  Q_OBJECT

public:
  explicit DialogSelectTopics(QWidget *parent = nullptr);
  ~DialogSelectTopics();

private:
  Ui::DialogSelectTopics *ui;
};

#endif // DIALOGSELECTTOPICS_H
