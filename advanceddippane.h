#ifndef ADVANCEDDIPPANE_H
#define ADVANCEDDIPPANE_H

#include <QWidget>

namespace Ui {
class AdvancedDipPane;
}

class StereonetWidget;
class StereonetControlPane;
class RoseControlPane;
class RoseWidget;

class AdvancedDipPane : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedDipPane(QWidget *parent = nullptr);
    ~AdvancedDipPane();

private slots:
    void onStackedWidgetChanged(int index);
    void onAddPlaneClicked();
    void onClearClicked();
    void onProjectionTypeChanged(int index);
    void onRadioButtonToggled(bool checked);
    void onDataBrowserBtnClicked();

private:
    void setupStereonetWrapper();
    void setupRoseWrapper();
    void setupControlPane();

    Ui::AdvancedDipPane *ui;
    
    // Stereonet 相关
    StereonetWidget *m_stereonetWidget = nullptr;
    StereonetControlPane *m_stereonetControlPane = nullptr;
    
    // Rose plot 相关
    RoseWidget* m_roseWidget = nullptr;
    RoseControlPane *m_roseControlPane = nullptr;
};

#endif // ADVANCEDDIPPANE_H
