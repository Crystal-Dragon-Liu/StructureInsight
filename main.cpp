#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "stereonetwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("EVStereonet - 构造分析绘图");
    mainWindow.resize(800, 800);
    
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 控制面板
    QWidget *controlPanel = new QWidget(centralWidget);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    
    QLabel *projectionLabel = new QLabel("投影类型:", controlPanel);
    QComboBox *projectionComboBox = new QComboBox(controlPanel);
    projectionComboBox->addItem("Equal-Area");
    projectionComboBox->addItem("Equal-Angle");
    
    QPushButton *addPlaneButton = new QPushButton("添加平面", controlPanel);
    QPushButton *clearButton = new QPushButton("清空", controlPanel);
    
    controlLayout->addWidget(projectionLabel);
    controlLayout->addWidget(projectionComboBox);
    controlLayout->addStretch();
    controlLayout->addWidget(addPlaneButton);
    controlLayout->addWidget(clearButton);
    
    // 立体网组件
    StereonetWidget *stereonetWidget = new StereonetWidget(centralWidget);
    stereonetWidget->setMinimumSize(600, 600);
    
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(stereonetWidget);
    
    mainWindow.setCentralWidget(centralWidget);
    
    // 连接信号槽
    QObject::connect(projectionComboBox, &QComboBox::currentIndexChanged, [=](int index) {
        StereonetType type = (index == 0) ? StereonetType::EqualArea : StereonetType::EqualAngle;
        stereonetWidget->setProjectionType(type);
    });
    
    QObject::connect(addPlaneButton, &QPushButton::clicked, [=]() {
        // 示例：添加一些测试平面
        Plane plane1;
        plane1.strike = 0.0; // 0度
        plane1.dip = M_PI / 6.0; // 30度
        stereonetWidget->addPlane(plane1);
        
        Plane plane2;
        plane2.strike = M_PI / 2.0; // 90度
        plane2.dip = M_PI / 4.0; // 45度
        stereonetWidget->addPlane(plane2);
        
        Plane plane3;
        plane3.strike = M_PI; // 180度
        plane3.dip = M_PI / 3.0; // 60度
        stereonetWidget->addPlane(plane3);
    });
    
    QObject::connect(clearButton, &QPushButton::clicked, [=]() {
        stereonetWidget->clearPlanes();
    });
    
    mainWindow.show();
    
    return app.exec();
}