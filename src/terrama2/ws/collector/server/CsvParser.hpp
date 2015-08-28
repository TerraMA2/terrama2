
//Std
#include <algorithm>

//Terralib
#include <terralib/datatype/Date.h>

//Qt
#include <QMap>
#include <QVariant>
#include <QDateTime>

/**
 * Simple parser for csv files.
 */
class CsvParser
{
    public:

        /**
         * Each data of the csv
         */
        struct DataCollection
        {
            QDateTime timeStamp;         //!< Date/Time
            QMap<QString, QVariant> values;   //!< Other Data

            inline bool operator< (const DataCollection & that) const { return this->timeStamp < that.timeStamp; }
        };

        /**
         * Constructor
         */
        CsvParser(QString filePath = "");

        void setPath(QString path)                   { filePath_ = path; }
        void setSeparator(QString separator)          { separator_ = separator; }
        void setHeaderSize(int headerSize)            { headerSize_ = headerSize_; }
        void setColumnNamesLine(int columnNamesLine)  { columnNamesLine_ = columnNamesLine; }
        void setDataStartLine(int dataStartLine)      { dataStartLine_ = dataStartLine;}
        void setDateColumnName(QString dateColumnName){ dateColumnName_ = dateColumnName; }
        void setDateTimeFormat(QString dateFormat)    { dateFormat_ = dateFormat; }

        bool parse();


        inline bool status() const { return status_; }


        inline const QString & errorMessage() const { return errMsg_; }

        inline const QList<DataCollection> & data() const { return data_; }


        bool getCollectionAtTime(QDateTime time, DataCollection&  data) const;

        bool hasCollectionAtTime(QDateTime time) const;

private:
        QString filePath_;
        QString separator_;
        QString dateColumnName_;
        QString dateFormat_;
        int headerSize_;
        int columnNamesLine_;
        int dataStartLine_;

        bool status_;
        QString errMsg_;

        QList<DataCollection> data_;
};
