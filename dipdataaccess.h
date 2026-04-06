#ifndef DIPDATAACCESS_H
#define DIPDATAACCESS_H

#include <QObject>
#include <QString>

struct DipData{
    float depth;
    float strike;
    QString type;
    float appDip;
    float appAzi;
    float trueDip;
    float trueAzi;
};

class DipDataAccess: public QObject
{
    Q_OBJECT
public:
    DipDataAccess(QObject* parent = nullptr);

    /*
     * @brief Fetch dip data.
    */
    bool fetchDataFromFile(const QString& dipFilePath);

    /*
     * @brief Get data
     */
    bool getDataByType(const QString& type, QVector<DipData>& output) const;
    bool getAllData(QVector<DipData>& data) const;
    bool getDipClassSet(QVector<QString>& data) const;
private:
    QVector<DipData> m_data;
};

#endif // DIPDATAACCESS_H
