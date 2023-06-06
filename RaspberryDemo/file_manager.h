#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include <QFile>
#include <QFileInfo>

using namespace std;

class file_manager : public QObject
{
    Q_OBJECT
public:
    explicit file_manager(QObject *parent = nullptr);


signals:

public slots:
    void write_Params(double gain, double offset);
    QVariantList read_Params();
    void xlsx_save(QString source, QVariantList folder); // name_string: serial number
    void file_export(QUrl filePath, QUrl folderName);


private:
    QString Params_path;


};

#endif // FILE_MANAGER_H
