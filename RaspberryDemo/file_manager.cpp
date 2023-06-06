#include "file_manager.h"
using namespace std;
using namespace QXlsx;

file_manager::file_manager(QObject *parent) : QObject(parent)
{
    Params_path= "/home/pi/Params.txt";

    QFile parameter_file(Params_path);
    if(!parameter_file.exists()){
        file_manager::write_Params(0,0);
    }
    else {
        file_manager::read_Params();
    }
}

void file_manager::write_Params(double gain, double offset)
{
    QFile parameter_file(Params_path);
    // create a file if the file was not there
    if(parameter_file.open(QIODevice::WriteOnly | QIODevice ::Text)){
       QTextStream txtstream(&parameter_file);
       txtstream << "Gain=\n";
       txtstream << QString::number(gain,'f',4);
       txtstream << "\n";
       txtstream << "Offset=\n";
       txtstream << QString::number(offset,'f',5);
       txtstream << "\n";
       parameter_file.close();
    }
    else {
         // Error : could not create param file
        qInfo() << "Error : could not create param file";
    }
}

QVariantList file_manager::read_Params()
{
    double gain, offset;
    QVariantList pair;
    QFile parameter_file(Params_path);
    if( parameter_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream txtstream(&parameter_file);
        QString Section_Name = "Val";
        QString Value = "0";
        // give value to each parameter
        while(!txtstream.atEnd()){
            Section_Name = Value;
            Value = txtstream.readLine();
            if(Section_Name == "Gain="){
                gain = Value.toDouble();
                pair << gain;
            }
            else if(Section_Name == "Offset="){
                offset = Value.toDouble();
                pair <<  offset;
            }
        }
        parameter_file.close();
    }
    else{
        // Error : can not open the parameters file
        qInfo() << "Error : can not open the parameters file";
    }
    return pair;
}

void file_manager::xlsx_save(QString name_string, QVariantList data)
{
    /* read the name of UUT and create a sheet in excell
     * data: List of Qstring [G+, G-, 0+, 0-, 180+, 180-, Natural Freq, Cut-off Freq, Over-shoot]
    */
    QString path ="/home/pi/";


    // wrtie date and time at top row
    QString date_string ;
    QString time_string;
    QDate date;
    QTime time;
    time = QTime::currentTime();
    date = QDate::currentDate(); // h:m:s ap
    date_string = date.toString("yyyy.MM.dd");
    time_string = time.toString("hh:mm ap");

    path.append(date_string);
    path.append(".xlsx");
    QFile file(path);
    QXlsx::Document xlsx(path);
    xlsx.addSheet(name_string);
    xlsx.selectSheet(name_string);
    xlsx.write("A1", "Date:"); //
    xlsx.write("B1", date_string); //
    xlsx.write("D1", "Time: "); //
    xlsx.write("E1", time_string); //
    xlsx.write("G1", "Case: "); //
    xlsx.write("H1", name_string); //
    // write titles
    xlsx.write("C3", "G+");
    xlsx.write("C4", "G-");
    xlsx.write("C5", "0+");
    xlsx.write("C6", "0-");
    xlsx.write("C7", "180+");
    xlsx.write("C8", "180-");
    xlsx.write("C9", "Natural Freq (Hz)");
    xlsx.write("C10", "Cut-off Freq (Hz)");
    xlsx.write("C11", "Overshoot (%)");
    // write values
    xlsx.write("D3", data[0].toString());
    xlsx.write("D4", data[1].toString());
    xlsx.write("D5", data[2].toString());
    xlsx.write("D6", data[3].toString());
    xlsx.write("D7", data[4].toString());
    xlsx.write("D8", data[5].toString());
    xlsx.write("D9", data[6].toString());
    xlsx.write("D10", data[7].toString());
    xlsx.write("D11", data[8].toString());


    xlsx.saveAs(path); // save the document as 'Test.xlsx'

//    ui->Console->setText("Saved Successfully");
//    console_timer->start(1000);

//    saved_items = saved_items + 1;
    //    ui->saved_num_lbl->setText(QString::number(saved_items));
}

void file_manager::file_export(QUrl source, QUrl folder)
{
    QString filePath = source.toLocalFile();

    QFileInfo fi(filePath);
    QString fileName = fi.fileName();
    QString folderName = folder.toLocalFile();
    qInfo() << "XLSX file: " << filePath << " Folder :" << folderName;
    qInfo() << " NAME: " << fileName;
    folderName.append("/");
    folderName.append(fileName);
    qInfo() << " Folder: " << folderName;

    while(QFile::exists(folderName)){
        folderName.prepend("copy_");
    }
    QFile::copy(filePath,folderName);

}
