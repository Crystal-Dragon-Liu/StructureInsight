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
// #include "intpropertyitem.h"
#include "multiselectpropertyitem.h"
#include "singleselectpropertyitem.h"
#include "stringpropertyitem.h"

const QString& PROPERTY_GEN_APPARENT_DIP_ITEM_NAME      = QObject::tr("Apparent Dip");
const QString& PROPERTY_GEN_TRUE_DIP_ITEM_NAME          = QObject::tr("True Dip");
const QString& PROPERTY_GEN_APPARENT_AZI_ITEM_NAME      = QObject::tr("Apparent Azimuth");
const QString& PROPERTY_GEN_TRUE_AZI_ITEM_NAME          = QObject::tr("True Azimuth");

#define DEBUG_MODE

AdvancedDipPane::AdvancedDipPane(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedDipPane), m_dataInterface(nullptr), m_propertyGroup(nullptr)
{
    ui->setupUi(this);
    
    // 设置 stackedWidget 默认显示 stereonetWrapper (索引0)
    ui->stackedWidget->setCurrentIndex(0);
    
    // 默认选中 stereonetBtn
    ui->stereonetBtn->setChecked(true);
    
    // 初始化各个组件
    setupStereonetWrapper();
    setupRoseWrapper();
    
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
    m_dataInterface = new DipDataAccess(this);
    // 构建属性面板
    initPropertyPanel();
    // 测试数据
    onDataBrowserBtnClicked();
}

void AdvancedDipPane::sltValueChangedWithName(const QString& propertyName, const QVariant& value){
    Q_UNUSED(propertyName);
    Q_UNUSED(propertyName);
}

void AdvancedDipPane::initPropertyPanel(){
    QLayout* layout = ui->rightPropertyPanelWrapper->layout();
    ui->rightPropertyPanelWrapper->setLayout(layout);
    m_propertyPanel = new PropertyPanel(ui->rightPropertyPanelWrapper);
    layout->addWidget(m_propertyPanel);

    // 通用属性
    m_propertyGroup = new PropertyGroup(tr("Data"), m_propertyPanel);
    m_propertyPanel->addGroup(m_propertyGroup);

    // 设置数据属性
    createProperty<StringPropertyItem>(PROPERTY_GEN_APPARENT_DIP_ITEM_NAME, tr("ApparentDip"));
    createProperty<StringPropertyItem>(PROPERTY_GEN_TRUE_DIP_ITEM_NAME,     tr("TrueDip"));
    createProperty<StringPropertyItem>(PROPERTY_GEN_APPARENT_AZI_ITEM_NAME, tr("ApparentAzimuth"));
    createProperty<StringPropertyItem>(PROPERTY_GEN_TRUE_AZI_ITEM_NAME,     tr("TrueAzimuth"));

    m_propertyPanel->addGroup(m_stereonetWidget->getPropertyGroup());
    m_propertyPanel->addGroup(m_roseWidget->getPropertyGroup());
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
    m_stereonetWidget->initProperties();
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
    m_roseWidget->initProperties();
    // 为 roseWrapper 添加一个占位标签（后续可以替换为 RosePlotWidget）
    QVBoxLayout *roseLayout = qobject_cast<QVBoxLayout*>(ui->roseWrapper->layout());
    if (roseLayout) {
        // 这里可以添加 RosePlotWidget
        // 目前先留空或添加占位符
        roseLayout->addWidget(m_roseWidget);
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
}

void AdvancedDipPane::onDataBrowserBtnClicked(){

#ifdef DEBUG_MODE
    QString filePath = "/Users/yunziyao/dev/StructureInsight/assets/dip_data_v3.csv";
    ui->lineEdit->setText(filePath);
#else
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
#endif

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
    // 更新数据到图件当中
    m_roseWidget->setData(*m_dataInterface);
    // TODO 更新数据到stereonet -> 董慧琨

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
