#include "dialogselecttopics.h"
#include "ui_dialogselecttopics.h"

DialogSelectTopics::DialogSelectTopics(
        QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectTopics)
{
    ui->setupUi(this);
}

DialogSelectTopics::~DialogSelectTopics()
{
    delete ui;
}
