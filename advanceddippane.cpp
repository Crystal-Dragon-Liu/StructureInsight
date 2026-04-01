#include "advanceddippane.h"
#include "ui_advanceddippane.h"
#include "stereonetwidget.h"
#include "stereonetcontrolpane.h"
#include "rosecontrolpane.h"
#include "rosewidget.h"
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QComboBox>
#include <QFileDialog>
#include <QDir>
#include "dipdataaccess.h"
#include <QListWidgetItem>
#include "propertypanel.h"
#include "propertygroup.h"
AdvancedDipPane::AdvancedDipPane(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedDipPane), m_dataInterface(nullptr)
{
    ui->setupUi(this);
    
    // 设置 stackedWidget 默认显示 stereonetWrapper (索引0)
    ui->stackedWidget->setCurrentIndex(0);
    
    // 默认选中 stereonetBtn
    ui->stereonetBtn->setChecked(true);
    
    // 初始化各个组件
    setupStereonetWrapper();
    setupRoseWrapper();
    setupControlPane();
    
    // 连接 stackedWidget 的切换信号
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, 
            this, &AdvancedDipPane::onStackedWidgetChanged);
    
    // 连接 radioButton 的信号
    connect(ui->stereonetBtn, &QRadioButton::toggled, 
            this, &AdvancedDipPane::onRadioButtonToggled);
    connect(ui->rosePlotBtn, &QRadioButton::toggled, 
            this, &AdvancedDipPane::onRadioButtonToggled);
    connect(ui->walkoutBtn, &QRadioButton::toggled, 
            this, &AdvancedDipPane::onRadioButtonToggled);
    connect(ui->crossSectionBtn, &QRadioButton::toggled, 
            this, &AdvancedDipPane::onRadioButtonToggled);
    connect(ui->browserBtn, &QPushButton::clicked,
            this, &AdvancedDipPane::onDataBrowserBtnClicked);
    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &AdvancedDipPane::onListWidgetItemClicked);
    m_dataInterface = new DipDataAccess();
    // 构建属性面板
    initPropertyPanel();
}

void AdvancedDipPane::initPropertyPanel(){
    QVBoxLayout* propertyLayout = new QVBoxLayout(ui->rightPropertyPanelWrapper);
    ui->rightPropertyPanelWrapper->setLayout(propertyLayout);
    m_propertyPanel = new PropertyPanel(ui->rightPropertyPanelWrapper);
    propertyLayout->addWidget(m_propertyPanel);
    // PropertyGroup* stereonetGroup = new PropertyGroup();
    // m_propertyPanel->addGroup();
}

AdvancedDipPane::~AdvancedDipPane()
{
    delete ui;
    delete m_dataInterface;
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
    m_roseWidget = new RoseWidget(ui->roseWrapper);
    m_roseWidget->setMinimumSize(200, 200); // 设置一个更小的最小尺寸
    m_roseWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 为 roseWrapper 添加一个占位标签（后续可以替换为 RosePlotWidget）
    QVBoxLayout *roseLayout = qobject_cast<QVBoxLayout*>(ui->roseWrapper->layout());
    if (roseLayout) {
        // 这里可以添加 RosePlotWidget
        // 目前先留空或添加占位符
        roseLayout->addWidget(m_roseWidget);
    }

    // 做一些模拟数据
    // 生成随机的strike数据
    QVector<int> strikes;
    int count = 120;
    // 生成一些随机数据，模拟不同方向的分布
    for (int i = 0; i < 60; i++) {
        strikes.append(QRandomGenerator::global()->bounded(360));
    }
    for (int i = 0; i < 20; i++) {
        strikes.append(QRandomGenerator::global()->bounded(10, 60));
    }
    for (int i = 0; i < 20; i++) {
        strikes.append(QRandomGenerator::global()->bounded(190, 300));
    }
    for (int i = 0; i < 20; i++) {
        strikes.append(QRandomGenerator::global()->bounded(60, 90));
    }
    m_roseWidget->setStrikes(strikes);
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
    
    // 移除现有的控制面板，但不删除它们，只是从布局中移除
    while (QLayoutItem *item = controlLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            // 从布局中移除，但不删除
            widget->setParent(nullptr);
        }
        delete item;
    }
    
    if (index == 0) {
        // 显示 stereonetWrapper，加载 StereonetControlPane
        if (!m_stereonetControlPane) {
            m_stereonetControlPane = new StereonetControlPane(this);
            
            // 只连接一次信号
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
        }
        // 设置父控件并添加到布局
        m_stereonetControlPane->setParent(ui->controlPane);
        controlLayout->addWidget(m_stereonetControlPane);
    } else if (index == 1) {
        // 显示 roseWrapper，加载 RoseControlPane
        if (!m_roseControlPane) {
            m_roseControlPane = new RoseControlPane(this);
        }
        // 设置父控件并添加到布局
        m_roseControlPane->setParent(ui->controlPane);
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

void AdvancedDipPane::onListWidgetItemClicked(QListWidgetItem *item){
    if(!item){
        return;
    }

    // 获取选中item的文本（即倾角类型）
    QString type = item->text();
    // 这里可以使用获取到的type进行后续操作
    qDebug() << "选中的倾角类型：" << type;
    QVector<DipData> typeData;
    this->m_dataInterface->getDataByType(type, typeData);
    QVector<int> strikes;
    foreach(auto dipData, typeData){
        strikes.push_back(static_cast<int>(dipData.strike));
    }
    m_roseWidget->setStrikes(strikes);
}

void AdvancedDipPane::onDataBrowserBtnClicked(){
    // 打开文件选择对话框，只允许选择.csv文件
    QDir currentDir = QDir::current();
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "choose dip vector data with .CSV format",
        currentDir.path(),
        "CSV file (*.csv);;All files (*.*)"
        );

    if (!filePath.isEmpty()) {
        // 更新lineEdit显示选中的文件路径
        ui->lineEdit->setText(filePath);
    }

    // 更新数据

    bool goodInput = this->m_dataInterface->fetchDataFromFile(filePath);
    if(!goodInput){
        return;
    }
    QVector<QString> defaultDipData;
    this->m_dataInterface->getDipClassSet(defaultDipData);

    // 更新数据到listWidget当中
    ui->listWidget->clear();
    // 添加倾角类型到listWidget
    foreach(auto type, defaultDipData) {
        ui->listWidget->addItem(type);
    }
}

void AdvancedDipPane::onRadioButtonToggled(bool checked)
{
    if (!checked) {
        return; // 只处理选中的情况
    }
    
    if (sender() == ui->stereonetBtn) {
        // 切换到 stereonetWrapper
        ui->stackedWidget->setCurrentIndex(0);
    } else if (sender() == ui->rosePlotBtn) {
        // 切换到 roseWrapper
        ui->stackedWidget->setCurrentIndex(1);
    } else if (sender() == ui->walkoutBtn) {
        // 暂不处理，因为还未实现
    } else if (sender() == ui->crossSectionBtn) {
        // 暂不处理，因为还未实现
    }
}
