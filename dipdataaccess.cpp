#include "dipdataaccess.h"
#include <QFile>
#include <QTextStream>
DipDataAccess::DipDataAccess() {}

bool DipDataAccess::fetchDataFromFile(const QString& dipFilePath){

    QFile file(dipFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }

    QTextStream in(&file);
    // 读取图头
    QString header = in.readLine();
    if(header.isEmpty()){
        file.close();
        return false;
    }

    // 检查图头有效性
    QStringList headerParts = header.split(",");
    if(headerParts.size() != 4){
        file.close();
        return false;
    }
    int depthIdx = -1;
    int dipIdx = -1;
    int strikeIdx = -1;
    int typeIdx = -1;
    for(int i = 0; i < headerParts.size(); i++){
        if(headerParts[i] == "DEPTH"){
            depthIdx = i;
        }
        else if(headerParts[i] == "DIP"){
            dipIdx = i;
        }
        else if(headerParts[i] == "STRIKE"){
            strikeIdx = i;
        }
        else if(headerParts[i] == "TYPE"){
            typeIdx = i;
        }
    }

    if(depthIdx < 0 || dipIdx < 0 || strikeIdx < 0 || typeIdx < 0){
        file.close();
        return false;
    }

    while (!in.atEnd()){
        // 读取一行
        QString line = in.readLine().trimmed();
        if(line.isEmpty()){
            continue;
        }
        // 按逗号分割成单元格
        QStringList cells = line.split(',');
        if(cells.size() != 4){
            file.close();
            continue;
        }

        DipData data;
        bool ok;
        data.depth = cells[depthIdx].toFloat(&ok);
        if(!ok){
            continue;
        }
        data.dip = cells[dipIdx].toFloat(&ok);
        if(!ok || data.dip < 0 || data.dip > 90.0){
            continue;
        }
        data.strike = cells[strikeIdx].toFloat(&ok);
        if(!ok || data.strike < 0 || data.strike > 360){
            continue;
        }
        data.type = cells[typeIdx];
        m_data.push_back(data);
    }
    file.close();
    return true;
}

bool DipDataAccess::getDataByType(const QString& type, QVector<DipData>& output) const{
    for (const DipData &data : m_data) {
        if (data.type == type) {
            output.append(data);
        }
    }
    return true;
}

bool DipDataAccess::getAllData(QVector<DipData>& data) const{
    data = m_data;
    return true;
}
