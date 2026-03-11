#include "advanceddippane.h"
#include "ui_advanceddippane.h"
#include "stereonetwidget.h"
#include "stereonetcontrolpane.h"
#include "rosecontrolpane.h"
#include <QVBoxLayout>
#include <QComboBox>

AdvancedDipPane::AdvancedDipPane(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedDipPane)
{
    ui->setupUi(this);
    
    // 设置 stackedWidget 默认显示 stereonetWrapper (索引0)
    ui->stackedWidget->setCurrentIndex(0);
    
    // 初始化各个组件
    setupStereonetWrapper();
    setupRoseWrapper();
    setupControlPane();
    
    // 连接 stackedWidget 的切换信号
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, 
            this, &AdvancedDipPane::onStackedWidgetChanged);
}

AdvancedDipPane::~AdvancedDipPane()
{
    delete ui;
}

void AdvancedDipPane::setupStereonetWrapper()
{
    // 在 stereonetWrapper 中创建 StereonetWidget，设置其父控件为 stereonetWrapper
    m_stereonetWidget = new StereonetWidget(ui->stereonetWrapper);
    // 移除固定的最小尺寸限制，让其能够根据窗口大小自由调整
    m_stereonetWidget->setMinimumSize(200, 200); // 设置一个更小的最小尺寸
    m_stereonetWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 添加到 stereonetWrapper 的布局中
    QVBoxLayout *stereonetLayout = qobject_cast<QVBoxLayout*>(ui->stereonetWrapper->layout());
    if (stereonetLayout) {
        stereonetLayout->addWidget(m_stereonetWidget);
    }
}

void AdvancedDipPane::setupRoseWrapper()
{
    // 为 roseWrapper 添加一个占位标签（后续可以替换为 RosePlotWidget）
    QVBoxLayout *roseLayout = qobject_cast<QVBoxLayout*>(ui->roseWrapper->layout());
    if (roseLayout) {
        // 这里可以添加 RosePlotWidget
        // 目前先留空或添加占位符
    }
}

void AdvancedDipPane::setupControlPane()
{
    // 根据当前显示的页面加载相应的控制面板
    onStackedWidgetChanged(ui->stackedWidget->currentIndex());
}

void AdvancedDipPane::onStackedWidgetChanged(int index)
{
    // 清除 controlPane 中现有的控件
    QVBoxLayout *controlLayout = qobject_cast<QVBoxLayout*>(ui->controlPane->layout());
    if (!controlLayout) {
        return;
    }
    
    // 删除现有的控制面板
    while (QLayoutItem *item = controlLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    
    if (index == 0) {
        // 显示 stereonetWrapper，加载 StereonetControlPane
        if (!m_stereonetControlPane) {
            m_stereonetControlPane = new StereonetControlPane(this);
        }
        controlLayout->addWidget(m_stereonetControlPane);
        
        // 连接 StereonetControlPane 的信号
        // 这里假设 StereonetControlPane 有相应的控件，我们通过 findChild 来获取
        if (QComboBox *projectionCombo = m_stereonetControlPane->findChild<QComboBox*>("projectionComboBox")) {
            connect(projectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this, &AdvancedDipPane::onProjectionTypeChanged);
        }
        if (QPushButton *addPlaneBtn = m_stereonetControlPane->findChild<QPushButton*>("addPlaneBtn")) {
            connect(addPlaneBtn, &QPushButton::clicked,
                    this, &AdvancedDipPane::onAddPlaneClicked);
        }
        if (QPushButton *clearBtn = m_stereonetControlPane->findChild<QPushButton*>("clearBtn")) {
            connect(clearBtn, &QPushButton::clicked,
                    this, &AdvancedDipPane::onClearClicked);
        }
    } else if (index == 1) {
        // 显示 roseWrapper，加载 RoseControlPane
        if (!m_roseControlPane) {
            m_roseControlPane = new RoseControlPane(this);
        }
        controlLayout->addWidget(m_roseControlPane);
    }
}

void AdvancedDipPane::onAddPlaneClicked()
{
    if (!m_stereonetWidget) {
        return;
    }
    
    // 示例：添加一些测试平面
    Plane plane1;
    plane1.strike = 0.0; // 0度
    plane1.dip = M_PI / 6.0; // 30度
    m_stereonetWidget->addPlane(plane1);
    
    Plane plane2;
    plane2.strike = M_PI / 2.0; // 90度
    plane2.dip = M_PI / 4.0; // 45度
    m_stereonetWidget->addPlane(plane2);
    
    Plane plane3;
    plane3.strike = M_PI; // 180度
    plane3.dip = M_PI / 3.0; // 60度
    m_stereonetWidget->addPlane(plane3);
}

void AdvancedDipPane::onClearClicked()
{
    if (m_stereonetWidget) {
        m_stereonetWidget->clearPlanes();
    }
}

void AdvancedDipPane::onProjectionTypeChanged(int index)
{
    if (!m_stereonetWidget) {
        return;
    }
    
    StereonetType type = (index == 0) ? StereonetType::EqualArea : StereonetType::EqualAngle;
    m_stereonetWidget->setProjectionType(type);
}
