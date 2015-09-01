
#include "CsvParser.hpp"

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QTimeZone>

CsvParser::CsvParser(QString filePath)
    : filePath_(filePath),
      separator_(","),
      headerSize_(0),
      columnNamesLine_(1),
      dataStartLine_(1),
      status_(false)
{
}

bool CsvParser::parse()
{
    if(columnNamesLine_ > dataStartLine_)
    {
        errMsg_ = QObject::tr("Data lines cannot start before column names.");
        return false;
    }

    filePath_ = filePath_.replace(QChar('\\'), QChar('/'));

    // Open file
    QFile file(filePath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errMsg_ = QObject::tr("Unable to open file: `%1'").arg(filePath_);
        return false;
    }

    // Parse column names
    QList<QString> attributeNames;

    // read each line:
    int currLine = 0;
    while (!file.atEnd())
    {
        currLine++;
        //Skip header
        if(currLine <  headerSize_)
            continue;

        //read line
        QString line(file.readLine());

        if (currLine == columnNamesLine_)
        {
            // Read column names.

            attributeNames = line.split(separator_,QString::KeepEmptyParts);

            if (attributeNames.empty())
            {
                errMsg_ = errMsg_ = QObject::tr("The CSV has no column names (%1, line %2).").arg(filePath_).arg(currLine);
                return false;
            }

        }

        if(currLine >= dataStartLine_)
        {
            DataCollection dataCollection;

            QStringList attributeValues = line.split(separator_,QString::KeepEmptyParts);

            //check number of columns
            if(attributeValues.size() != attributeNames.size())
            {
                errMsg_ = QObject::tr("File error: Number of values does not match the number of columns.");
                return false;
            }

            for(int i = 0, size = attributeNames.size(); i < size; ++i)
            {
                QString name  = attributeNames.at(i).trimmed();
                QString value = attributeValues.at(i).trimmed();

                if(name == dateColumnName_) //read time
                    dataCollection.timeStamp = QDateTime::fromString(value, dateFormat_);
                else //read value
                    dataCollection.values.insert(name, value);

            }

            //append data
            data_.append(dataCollection);
        }
    }

    // Sort data based on dates
    std::sort(data_.begin(), data_.end());

    // OK!
    status_ = true;

    return true;
}




bool CsvParser::getCollectionAtTime(QDateTime time, CsvParser::DataCollection & data) const
{
    for(const DataCollection& d : data_)
    {
        if (d.timeStamp == time)
        {
            data = d;
            return true;
        }
    }

    return false;
}

bool CsvParser::hasCollectionAtTime(QDateTime time) const
{
    for(const DataCollection& d : data_)
    {
        if (d.timeStamp == time)
        {
            // Achamos!
            return true;
        }
    }

    // Não achamos:
    return false;
}
