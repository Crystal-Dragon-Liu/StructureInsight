#ifndef STEREONETCONTROLPANE_H
#define STEREONETCONTROLPANE_H

#include <QWidget>

namespace Ui {
class StereonetControlPane;
}

class StereonetControlPane : public QWidget
{
    Q_OBJECT

public:
    explicit StereonetControlPane(QWidget *parent = nullptr);
    ~StereonetControlPane();

private:
    Ui::StereonetControlPane *ui;
};

#endif // STEREONETCONTROLPANE_H
