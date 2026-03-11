#include "rosecontrolpane.h"
#include "ui_rosecontrolpane.h"

RoseControlPane::RoseControlPane(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoseControlPane)
{
    ui->setupUi(this);
}

RoseControlPane::~RoseControlPane()
{
    delete ui;
}
