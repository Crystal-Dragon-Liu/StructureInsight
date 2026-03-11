#ifndef ROSECONTROLPANE_H
#define ROSECONTROLPANE_H

#include <QWidget>

namespace Ui {
class RoseControlPane;
}

class RoseControlPane : public QWidget
{
    Q_OBJECT

public:
    explicit RoseControlPane(QWidget *parent = nullptr);
    ~RoseControlPane();

private:
    Ui::RoseControlPane *ui;
};

#endif // ROSECONTROLPANE_H
