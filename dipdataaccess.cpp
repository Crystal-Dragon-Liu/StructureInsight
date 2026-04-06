#include "dipdataaccess.h"
#include <QFile>
#include <QTextStream>
#include <set>

// 可能的数据项名称，临时用

const QStringList& DEPTH_NAME_LIST = {"DEPTH", "DEPT"};
const QStringList& APP_DIP_NAME_LIST = {"DIP", "APPDIP"};
const QStringList& TRUE_DIP_NAME_LIST = {"DIP_TRUE", "TRUE_DIP", "TRDIP"};
const QStringList& APP_AZI_NAME_LIST = {"APP_AZI", "AZIMUTH_NP"};
const QStringList& TRUE_AZI_NAME_LIST = {"AZIMUTH_TRUE"};
const QStringList& HAZI_NAME_LIST = {"HAZI"};
const QStringList& DEVI_NAME_LIST = {"DEVI"};
const QStringList& TYPE_NAME_LIST = {"TYPE"};

DipDataAccess::DipDataAccess(QObject* parent): QObject(parent) {}

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
    // 索引列表 - Depth -> [0], Apparent Dip -> [1], Apparent Azimuth -> [2], True Dip -> [3], True Azimuth -> [4], Type -> [5]
    std::vector<int> itemIdxVec = {-1, -1, -1, -1, -1, -1};
    for(int i = 0; i < headerParts.size(); i++){
        if(DEPTH_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))           { itemIdxVec[0] = i;}
        else if(APP_DIP_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))    { itemIdxVec[1] = i;}
        else if(APP_AZI_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))    { itemIdxVec[2] = i;}
        else if(TRUE_DIP_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))   { itemIdxVec[3] = i;}
        else if(TRUE_AZI_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))   { itemIdxVec[4] = i;}
        else if(TYPE_NAME_LIST.contains(headerParts[i], Qt::CaseInsensitive))       { itemIdxVec[5] = i;}
    }

    while (!in.atEnd()){
        // 读取一行
        QString line = in.readLine().trimmed();
        if(line.isEmpty()){ continue; }
        // 按逗号分割成单元格
        QStringList cells = line.split(',');
        if(cells.size() != 4){
            file.close();
            continue;
        }

        DipData data;
        bool ok;
        // 获取深度
        data.depth = cells[0].toFloat(&ok);
        if(!ok){ continue;}
        // 获取视倾角
        data.appDip = cells[1].toFloat(&ok);
        if(!ok || data.appDip < 0.f || data.appDip > 90.0f){
            continue;
        }
        // 获取视倾向
        data.appAzi = cells[2].toFloat(&ok);
        if(!ok || data.appAzi < 0.f || data.appAzi > 360.f){
            continue;
        }
        // 获取真倾角
        data.trueDip = cells[3].toFloat(&ok);
        if(!ok || data.trueDip < 0.f || data.trueDip > 90.0f){
            continue;
        }
        data.trueAzi = cells[4].toFloat(&ok);
        if(!ok || data.trueAzi < 0.f || data.trueAzi > 360.0f){
            continue;
        }
        if(ok){
            // 默认走向使用真倾向进行计算
            data.strike = data.trueAzi - 90.f;
            if(data.strike < 0.f){
                data.strike += 360.0f;
            }
        }
        data.type = cells[5];
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

bool DipDataAccess::getDipClassSet(QVector<QString>& data) const{
    std::set<QString> dipSet;
    foreach(auto dip, m_data){
        dipSet.insert(dip.type);
    }
    if(dipSet.empty()){
        return false;
    }
    for (auto it = dipSet.begin(); it != dipSet.end(); ++it) {
        data.push_back(*it);
    }
    return true;
}
