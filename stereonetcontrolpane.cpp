#include "stereonetcontrolpane.h"
#include "ui_stereonetcontrolpane.h"

StereonetControlPane::StereonetControlPane(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StereonetControlPane)
{
    ui->setupUi(this);
}

StereonetControlPane::~StereonetControlPane()
{
    delete ui;
}
