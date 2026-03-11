#include <QApplication>
#include <QMainWindow>
#include "advanceddippane.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("EVStereonet - 构造分析绘图");
    mainWindow.resize(1000, 900);
    
    // 使用 AdvancedDipPane 作为主窗口的中央部件
    AdvancedDipPane *advancedDipPane = new AdvancedDipPane(&mainWindow);
    mainWindow.setCentralWidget(advancedDipPane);
    
    mainWindow.show();
    
    return app.exec();
}