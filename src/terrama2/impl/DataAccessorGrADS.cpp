/*
 Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

 This file is part of TerraMA2 - a free and open source computational
 platform for analysis, monitoring, and alert of geo-environmental extremes.

 TerraMA2 is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License,
 or (at your option) any later version.

 TerraMA2 is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with TerraMA2. See LICENSE. If not, write to
 TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
 */

/*!
  \file terrama2/core/data-access/DataAccessorGrADS.cpp

  \brief Data accessor for GrADS format

  \author Paulo R. M. Oliveira
 */

#include "DataAccessorGrADS.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Utils.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Raii.hpp"
#include "../core/utility/Unpack.hpp"

//TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/raster/RasterProperty.h>
#include <terralib/raster/BandProperty.h>
#include <terralib/raster/Grid.h>
#include <terralib/raster/RasterFactory.h>
#include <terralib/raster/Band.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/srs/SpatialReferenceSystemManager.h>
#include <terralib/srs/SpatialReferenceSystem.h>

//QT
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QObject>
#include <QFileInfo>
#include <QUrl>
#include <QDir>


terrama2::core::DataAccessorGrADS::DataAccessorGrADS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries,
                                                     const Filter& filter)
    : DataAccessor(dataProvider, dataSeries, filter),
      DataAccessorGrid(dataProvider, dataSeries, filter),
      DataAccessorFile(dataProvider, dataSeries, filter)
{
  if (dataSeries->semantics.code != "GRID-grads")
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException() << ErrorDescription(errMsg);
  }
}


std::string terrama2::core::DataAccessorGrADS::getCtlFilename(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("ctl_filename");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined CTL filename in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorGrADS::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataset,
                                                            const Filter& filter) const
{
  std::string mask = getCtlFilename(dataset);
  std::string uri = dataRetriever->retrieveData(mask, filter);

  QUrl url(QString::fromStdString(uri));
  auto gradsDescriptor = readDataDescriptor(url.path().toStdString()+"/"+mask);

  std::string datasetMask = gradsDescriptor.datasetFilename_;
  if (gradsDescriptor.datasetFilename_[0] == '^')
  {
    gradsDescriptor.datasetFilename_.erase(0, 1);
    datasetMask = gradsDescriptor.datasetFilename_;
  }

  datasetMask = replaceMask(datasetMask.c_str()).toStdString();

  uri = dataRetriever->retrieveData(datasetMask, filter);

  return uri;
}

std::string terrama2::core::DataAccessorGrADS::dataSourceType() const
{ return "GDAL"; }

std::shared_ptr<te::da::DataSet>
terrama2::core::DataAccessorGrADS::createCompleteDataSet(std::shared_ptr<te::da::DataSetType> dataSetType) const
{
  te::dt::Property* timestamp = new te::dt::DateTimeProperty("file_timestamp", te::dt::TIME_INSTANT_TZ);
  dataSetType->add(timestamp);
  return std::make_shared<te::mem::DataSet>(dataSetType.get());
}

void terrama2::core::DataAccessorGrADS::addToCompleteDataSet(std::shared_ptr<te::da::DataSet> completeDataSet,
                                                             std::shared_ptr<te::da::DataSet> dataSet,
                                                             std::shared_ptr<te::dt::TimeInstantTZ> fileTimestamp) const
{
  auto complete = std::dynamic_pointer_cast<te::mem::DataSet>(completeDataSet);
  complete->moveLast();

  size_t rasterColumn = te::da::GetFirstPropertyPos(dataSet.get(), te::dt::RASTER_TYPE);
  if (!isValidColumn(rasterColumn))
  {
    QString errMsg = QObject::tr("No raster attribute.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  size_t timestampColumn = te::da::GetFirstPropertyPos(complete.get(), te::dt::DATETIME_TYPE);

  dataSet->moveBeforeFirst();
  while (dataSet->moveNext())
  {
    std::unique_ptr<te::rst::Raster> raster(
        dataSet->isNull(rasterColumn) ? nullptr : dataSet->getRaster(rasterColumn).release());

    te::mem::DataSetItem* item = new te::mem::DataSetItem(complete.get());

    item->setRaster(rasterColumn, raster.release());
    if (isValidColumn(timestampColumn))
      item->setDateTime(timestampColumn,
                        fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    complete->add(item);
  }
}

QString terrama2::core::DataAccessorGrADS::replaceMask(QString mask) const
{

  /*
   * GrADS date format
    %y2  - yy    2 digit year
    %y4  - yyyy  4 digit year
    %m2  - MM    2 digit month (leading zero if needed)
    %d2  - dd    2 digit day (leading zero if needed)
    %h2  - hh    2 digit hour
    %n2  - mm    2 digit minute; leading zero if needed
  */

  mask = mask.replace("%y2", "yy");
  mask = mask.replace("%y4", "yyyy");
  mask = mask.replace("%m2", "MM");
  mask = mask.replace("%d2", "dd");
  mask = mask.replace("%h2", "hh");
  mask = mask.replace("%n2", "mm");

  return mask;
}

terrama2::core::DataSetSeries terrama2::core::DataAccessorGrADS::getSeries(const std::string& uri,
                                                                           const terrama2::core::Filter& filter,
                                                                           terrama2::core::DataSetPtr dataSet) const
{
  QUrl url;
  try
  {
    std::string folder = getFolder(dataSet);
    url = QUrl(QString::fromStdString(uri + "/" + folder));
  }
  catch (UndefinedTagException&)
  {
    url = QUrl(QString::fromStdString(uri));
  }

  QDir dir(url.path());
  QFileInfoList fileInfoList = dir.entryInfoList(
      QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
  if (fileInfoList.empty())
  {
    QString errMsg = QObject::tr("No file in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NoDataException() << ErrorDescription(errMsg);
  }

  //return value
  DataSetSeries series;
  series.dataSet = dataSet;

  std::shared_ptr<te::da::DataSet> completeDataset(nullptr);
  std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);


  boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
  std::shared_ptr<te::dt::TimeInstantTZ> lastFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);

  //get timezone of the dataset
  std::string timezone;
  try
  {
    timezone = getTimeZone(dataSet);
  }
  catch (const terrama2::core::UndefinedTagException& /*e*/)
  {
    //if timezone is not defined
    timezone = "UTC+00";
  }

  //fill file list
  QFileInfoList newFileInfoList;
  for(const auto& fileInfo : fileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();
    std::string folderPath = dir.absolutePath().toStdString();
    if(terrama2::core::Unpack::verifyCompressFile(folderPath+ "/" + name))
    {
      //unpack files
      std::string tempFolderPath = terrama2::core::Unpack::unpackList(folderPath+ "/" + name);
      QDir tempDir(QString::fromStdString(tempFolderPath));
      QFileInfoList fileList = tempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

      newFileInfoList.append(fileList);
    }
    else
    {
      newFileInfoList.append(fileInfo);
    }
  }

  bool first = true;
  for (const auto& fileInfo : newFileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();

    std::shared_ptr<te::dt::TimeInstantTZ> thisFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);

    QString ctlMask = getCtlFilename(dataSet).c_str();

    // Verify if it is a valid CTL file name
    if (!isValidDataSetName(ctlMask.toStdString(), filter, timezone, name, thisFileTimestamp))
      continue;

    auto gradsDescriptor = readDataDescriptor(fileInfo.absoluteFilePath().toStdString());;
    gradsDescriptor.srid_ = getSrid(dataSet);

    //FIXME: temporary restriction of only one band
    if(gradsDescriptor.tDef_->numValues_ > 1)
    {
      QString errMsg = QObject::tr("Invalid number of bands in dataset %1").arg(dataSet->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataAccessException() << ErrorDescription(errMsg);
    }

    // Reads the dataset name from CTL
    std::string datasetMask = gradsDescriptor.datasetFilename_;
    if (gradsDescriptor.datasetFilename_[0] == '^')
    {
      gradsDescriptor.datasetFilename_.erase(0, 1);
      datasetMask = gradsDescriptor.datasetFilename_;
    }

    datasetMask = replaceMask(datasetMask.c_str()).toStdString();


    for (const auto& dataFileInfo : newFileInfoList)
    {
      std::string name = dataFileInfo.fileName().toStdString();
      std::string baseName = dataFileInfo.baseName().toStdString();
      std::string extension = dataFileInfo.suffix().toStdString();

      // Verify if the file name matches the datasetMask
      if (!isValidDataSetName(datasetMask, filter, timezone, name, thisFileTimestamp))
        continue;

      boost::replace_last(name, extension, "vrt");

      QString binFilename = dataFileInfo.absoluteFilePath();
      QString vrtFilename = dataFileInfo.absoluteFilePath();
      vrtFilename.replace(QString::fromStdString(extension), "vrt");


      writeVRTFile(gradsDescriptor, binFilename.toStdString(), vrtFilename.toStdString());

      // creates a DataSource to the data and filters the dataset,
      // also joins if the DCP comes from separated files
      std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType()));
      std::map<std::string, std::string> connInfo;

      connInfo["URI"] = typePrefix() + dataFileInfo.absolutePath().toStdString() + "/" + name;
      datasource->setConnectionInfo(connInfo);

      //RAII for open/closing the datasource
      OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

      if (!datasource->isOpened())
      {
        // Can't throw here, inside loop
        // just log and continue
        QString errMsg = QObject::tr("DataProvider could not be opened.");
        TERRAMA2_LOG_ERROR() << errMsg;
        continue;
      }

      // get a transactor to interact to the data source
      std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

      // Some drivers use the base name and other use filename with extension
      std::string dataSetName;
      std::vector<std::string> dataSetNames = transactor->getDataSetNames();
      auto itBaseName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), baseName);
      auto itFileName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), name);
      if (itBaseName != dataSetNames.cend())
        dataSetName = baseName;
      else if (itFileName != dataSetNames.cend())
        dataSetName = name;
      //No valid dataset name found
      if (dataSetName.empty())
        continue;

      std::shared_ptr<te::da::DataSet> teDataSet(transactor->getDataSet(dataSetName));
      if(!teDataSet)
      {
        QString errMsg = QObject::tr("Could not read dataset: %1").arg(dataSetName.c_str());
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
      }
      auto raster = teDataSet->getRaster(0);
      if(raster.get() == nullptr)
      {
        QString errMsg = QObject::tr("Invalid raster for dataset: %1").arg(dataSetName.c_str());
        TERRAMA2_LOG_WARNING() << errMsg;
        throw terrama2::core::DataAccessorException() << ErrorDescription(errMsg);
      }


      if (first)
      {
        //read and adapt all te:da::DataSet from terrama2::core::DataSet
        converter = getConverter(dataSet, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(dataSetName)));
        series.teDataSetType.reset(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
        assert(series.teDataSetType.get());
        completeDataset = createCompleteDataSet(series.teDataSetType);
        first = false;
      }

      addToCompleteDataSet(completeDataset, teDataSet, thisFileTimestamp);
//      // Creates flipped raster
//      QFile vrtFile(vrtFilename);
//      vrtFile.remove();


      //update last file timestamp
      if (lastFileTimestamp->getTimeInstantTZ().is_not_a_date_time() || *lastFileTimestamp < *thisFileTimestamp)
        lastFileTimestamp = thisFileTimestamp;

    }
  }

  if (!completeDataset.get() || completeDataset->isEmpty())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_WARNING() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }

  filterDataSet(completeDataset, filter);

  //Get last data timestamp and compare with file name timestamp
  std::shared_ptr<te::dt::TimeInstantTZ> dataTimeStamp = getDataLastTimestamp(dataSet, completeDataset);

  filterDataSetByLastValue(completeDataset, filter, dataTimeStamp);

  //if both dates are valid
  if ((lastFileTimestamp.get() && !lastFileTimestamp->getTimeInstantTZ().is_not_a_date_time())
      && (dataTimeStamp.get() && !dataTimeStamp->getTimeInstantTZ().is_not_a_date_time()))
  {
    (*lastDateTime_) = *dataTimeStamp > *lastFileTimestamp ? *dataTimeStamp : *lastFileTimestamp;
  }
  else if (lastFileTimestamp.get() && !lastFileTimestamp->getTimeInstantTZ().is_not_a_date_time())
  {
    //if only fileTimestamp is valid
    (*lastDateTime_) = *lastFileTimestamp;
  }
  else if (dataTimeStamp.get() && !dataTimeStamp->getTimeInstantTZ().is_not_a_date_time())
  {
    //if only dataTimeStamp is valid
    (*lastDateTime_) = *dataTimeStamp;
  }
  else
  {
    boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
    (*lastDateTime_) = te::dt::TimeInstantTZ(noTime);
  }


  std::shared_ptr<SynchronizedDataSet> syncDataset(new SynchronizedDataSet(completeDataset));
  series.syncDataSet = syncDataset;
  return series;
}


void terrama2::core::GrADSDataDescriptor::addVar(const std::string& strVar)
{
  Var* var = new Var;
  QStringList tokens = QString::fromStdString(strVar).split(" ");

  if (tokens.size() >= 4)
  {
    var->varName_ = tokens[0].toStdString();
    var->verticalLevels_ = tokens[1].toInt();
    var->units_ = tokens[2].toStdString();

    // Description may have spaces, need to concatenate all pieces
    for (unsigned int i = 3; i < tokens.size(); ++i)
    {
      if (!var->description_.empty())
        var->description_ += " ";
      var->description_ += tokens[i].toStdString();
    }
  }
  else
  {
    delete var;
    QString errMsg = QObject::tr("Wrong number of fields in VAR configuration, expected at least 4");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  vecVars_.push_back(var);
}

void terrama2::core::GrADSDataDescriptor::setKeyValue(const std::string& key, const std::string& value)
{
  bool found = false;
  if (key == "DSET")
  {
    datasetFilename_ = value;
    found = true;
  }
  else if (key == "TITLE")
  {
    title_ = value;
    found = true;
  }
  else if (key == "UNDEF")
  {
    undef_ = std::atof(value.c_str());
    found = true;
  }
  else if (key == "OPTIONS" || key == "*OPTIONS")
  {
    QStringList tokens = QString::fromStdString(value).split(" ");
    for (QString token : tokens)
    {
      vecOptions_.push_back(token.toStdString());
    }

    found = true;
  }
  else if (key == "XDEF")
  {
    xDef_ = getValueDef(value, "X");
    if (xDef_ != nullptr)
      found = true;
  }
  else if (key == "YDEF")
  {
    yDef_ = getValueDef(value, "Y");
    if (yDef_ != nullptr)
      found = true;
  }
  else if (key == "ZDEF")
  {
    zDef_ = getValueDef(value, "Z");
    if (zDef_ != nullptr)
      found = true;
  }
  else if (key == "TDEF")
  {
    tDef_ = getTValueDef(value);
    if (tDef_ != nullptr)
      found = true;
  }
  //TODO: Implement rest of the parameters

  if (!found)
  {
    QString errMsg = QObject::tr("Could not find a valid tag for the given key: %1").arg(key.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }
}

terrama2::core::GrADSDataDescriptor::TValueDef*
terrama2::core::GrADSDataDescriptor::getTValueDef(const std::string& value)
{
  TValueDef* valueDef = new TValueDef;

  QStringList tokens = QString::fromStdString(value).split(" ");
  if (tokens.size() < 3)
  {
    delete valueDef;
    QString errMsg = QObject::tr("Wrong number of fields in TDEF configuration, expected at least 3");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  valueDef->numValues_ = tokens[0].toInt();
  if (tokens[1].toUpper() == "LINEAR")
    valueDef->dimensionType_ = LINEAR;
  else if (tokens[1].toUpper() == "LEVELS")
    valueDef->dimensionType_ = LEVELS;
  else
  {
    delete valueDef;
    QString errMsg = QObject::tr("Invalid mapping method for TDEF");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  for (int i = 2; i < tokens.size(); ++i)
  {
    valueDef->values_.push_back(tokens[i].toStdString());
  }

  return valueDef;
}

terrama2::core::GrADSDataDescriptor::ValueDef*
terrama2::core::GrADSDataDescriptor::getValueDef(const std::string& value, const std::string& dimension)
{
  ValueDef* valueDef = new ValueDef;

  QStringList tokens = QString::fromStdString(value).split(" ");
  if (tokens.size() < 3)
  {
    delete valueDef;
    QString errMsg = QObject::tr("Wrong number of fields in %1DEF configuration, expected at least 3").arg(
        dimension.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  valueDef->numValues_ = tokens[0].toInt();
  std::string token = tokens[1].toUpper().toStdString();
  if (token == "LINEAR")
    valueDef->dimensionType_ = LINEAR;
  else if (token == "LEVELS")
    valueDef->dimensionType_ = LEVELS;
  else if (token == "GAUST62" && dimension == "Y")
    valueDef->dimensionType_ = GAUST62;
  else if (token == "GAUSR15" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR15;
  else if (token == "GAUSR20" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR20;
  else if (token == "GAUSR30" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR30;
  else if (token == "GAUSR40" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR40;
  else
  {
    delete valueDef;
    QString errMsg = QObject::tr("Invalid mapping method for %1DEF").arg(dimension.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  for (int i = 2; i < tokens.size(); ++i)
  {
    valueDef->values_.push_back(tokens[i].toFloat());
  }

  return valueDef;
}

terrama2::core::GrADSDataDescriptor& terrama2::core::GrADSDataDescriptor::operator=(const GrADSDataDescriptor& rhs)
{
  if (this == &rhs)
    return *this;

  // Delete pointers from own copy
  for (size_t i = 0; i < vecCHSUB_.size(); ++i)
    delete vecCHSUB_[i];
  vecCHSUB_.clear();

  if (xDef_ != nullptr)
  {
    delete xDef_;
    xDef_ = nullptr;
  }

  if (yDef_ != nullptr)
  {
    delete yDef_;
    yDef_ = nullptr;
  }

  if (zDef_ != nullptr)
  {
    delete zDef_;
    zDef_ = nullptr;
  }

  if (tDef_ != nullptr)
  {
    delete tDef_;
    zDef_ = nullptr;
  }

  for (size_t i = 0; i < vecVars_.size(); ++i)
    delete vecVars_[i];
  vecVars_.clear();

  //copy values from rhs
  datasetFilename_ = rhs.datasetFilename_;
  title_ = rhs.title_;
  vecOptions_ = rhs.vecOptions_;
  undef_ = rhs.undef_;
  fileHeaderLength_ = rhs.fileHeaderLength_;
  srid_ = rhs.srid_;

  for (size_t i = 0; i < rhs.vecCHSUB_.size(); ++i)
    vecCHSUB_.push_back(new CHSUB(*rhs.vecCHSUB_[i]));

  if (rhs.xDef_ != nullptr)
    xDef_ = new ValueDef(*rhs.xDef_);

  if (rhs.yDef_ != nullptr)
    yDef_ = new ValueDef(*rhs.yDef_);

  if (rhs.zDef_ != nullptr)
    zDef_ = new ValueDef(*rhs.zDef_);

  if (rhs.tDef_ != nullptr)
    tDef_ = new TValueDef(*rhs.tDef_);

  for (size_t i = 0; i < rhs.vecVars_.size(); ++i)
    vecVars_.push_back(new Var(*rhs.vecVars_[i]));

  return *this;
}

terrama2::core::GrADSDataDescriptor::GrADSDataDescriptor(const GrADSDataDescriptor& rhs)
{
  datasetFilename_ = rhs.datasetFilename_;
  title_ = rhs.title_;
  vecOptions_ = rhs.vecOptions_;
  undef_ = rhs.undef_;
  fileHeaderLength_ = rhs.fileHeaderLength_;
  srid_ = rhs.srid_;

  for (size_t i = 0; i < rhs.vecCHSUB_.size(); ++i)
    vecCHSUB_.push_back(new CHSUB(*rhs.vecCHSUB_[i]));

  if (rhs.xDef_ != nullptr)
    xDef_ = new ValueDef(*rhs.xDef_);

  if (rhs.yDef_ != nullptr)
    yDef_ = new ValueDef(*rhs.yDef_);

  if (rhs.zDef_ != nullptr)
    zDef_ = new ValueDef(*rhs.zDef_);

  if (rhs.tDef_ != nullptr)
    tDef_ = new TValueDef(*rhs.tDef_);

  for (size_t i = 0; i < rhs.vecVars_.size(); ++i)
    vecVars_.push_back(new Var(*rhs.vecVars_[i]));
}

terrama2::core::GrADSDataDescriptor::~GrADSDataDescriptor()
{
  for (size_t i = 0; i < vecCHSUB_.size(); ++i)
    delete vecCHSUB_[i];

  if (xDef_ != nullptr)
    delete xDef_;

  if (yDef_ != nullptr)
    delete yDef_;

  if (zDef_ != nullptr)
    delete zDef_;

  if (tDef_ != nullptr)
    delete tDef_;

  for (size_t i = 0; i < vecVars_.size(); ++i)
    delete vecVars_[i];
}

terrama2::core::GrADSDataDescriptor::GrADSDataDescriptor() : undef_(0.),
                                                             fileHeaderLength_(0),
                                                             xDef_(nullptr),
                                                             yDef_(nullptr),
                                                             zDef_(nullptr),
                                                             tDef_(nullptr),
                                                             srid_(0)
{
}


terrama2::core::GrADSDataDescriptor
terrama2::core::DataAccessorGrADS::readDataDescriptor(const std::string& filename) const
{
  GrADSDataDescriptor grADSDataDescriptor;

  // Opens the CTL file
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString errMsg = QObject::tr("Could not read ctl file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  // Reads the dataset name from CTL

  std::string section, key, value;
  enum ParseState
  {
    FindSection, FindKey, FindVar
  };

  ParseState state = FindSection;

  std::string line;
  QTextStream in(&file);
  while (!in.atEnd())
  {
    switch (state)
    {
      case FindSection:
      {
        line = trim(in.readLine().toStdString());
        if (line.empty())
          continue;

        std::string key(line);
        std::string value;
        std::size_t found = line.find(" ");
        if (found != std::string::npos)
        {
          key = line.substr(0, found);
          value = line.substr(found + 1);
        }

        boost::to_upper(key);

        if (key == "VARS")
        {
          section = "VARS";
          state = FindVar;
        }
        else if (key == "ENDVARS")
        {
          section = "";
          state = FindSection;
          continue;
        }
        else
        {
          section = "";
          state = FindKey;
        }
        break;
      }
      case FindKey:
      {
        if (line.empty())
        {
          state = FindSection;
          continue;
        }

        std::string key(line);
        std::string value;
        std::size_t found = line.find(" ");
        if (found != std::string::npos)
        {
          key = line.substr(0, found);
          value = line.substr(found + 1);
        }

        boost::to_upper(key);

        grADSDataDescriptor.setKeyValue(key, value);
        state = FindSection;
        break;
      }
      case FindVar:
      {
        line = trim(in.readLine().toStdString());

        if (line.empty())
          continue;

        if (line == "ENDVARS")
        {
          state = FindSection;
          continue;
        }

        grADSDataDescriptor.addVar(line);
      }
    }
  }

  return grADSDataDescriptor;
}

void terrama2::core::DataAccessorGrADS::writeVRTFile(terrama2::core::GrADSDataDescriptor descriptor,
                                                     const std::string& binFilename,
                                                     const std::string& vrtFilename) const
{

  std::ofstream vrtfile;
  vrtfile.open(vrtFilename);

  if (!vrtfile.is_open())
  {
    QString errMsg = QObject::tr("Could not open VRT file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }
  else
  {
    vrtfile.precision(20);

    vrtfile
        << "<VRTDataset rasterXSize=\"" << descriptor.xDef_->numValues_ << "\""
        << " rasterYSize=\"" << descriptor.yDef_->numValues_ << "\""
        << ">";

    if (descriptor.srid_ > 0)
    {
      const std::string wktStr = te::srs::SpatialReferenceSystemManager::getInstance().getWkt(descriptor.srid_);

      if (!wktStr.empty())
      {
        vrtfile << std::endl << "<SRS>" << wktStr << "</SRS>";
      }
    }

    if ((descriptor.xDef_->values_[1] != 0.0) && (descriptor.yDef_->values_[1] != 0.0))
    {
      vrtfile << std::endl << "<GeoTransform>" << descriptor.xDef_->values_[0] << ","
              << descriptor.xDef_->values_[1] << ",0," << descriptor.yDef_->values_[0] << ",0,"
              << descriptor.yDef_->values_[1]
              << "</GeoTransform>";
    }

    unsigned int pixelOffset = 0;
    unsigned int lineOffset = 0;
    unsigned int imageOffset = 0;

    for (unsigned int bandIdx = 0; bandIdx < descriptor.tDef_->numValues_; ++bandIdx)
    {

      // BSQ (Band sequential) interleave
      pixelOffset = 4; // Float32
      lineOffset = pixelOffset * descriptor.xDef_->numValues_;
      imageOffset = bandIdx * lineOffset * descriptor.xDef_->numValues_;

      vrtfile
          << std::endl
          << "<VRTRasterBand dataType=\"Float32\" band=\"" << (bandIdx + 1) << "\" subClass=\"VRTRawRasterBand\">"
          << std::endl
          << "<SourceFilename relativetoVRT=\"1\">" << binFilename << "</SourceFilename>" << std::endl
          << "<ImageOffset>" << imageOffset << "</ImageOffset>" << std::endl
          << "<PixelOffset>" << pixelOffset << "</PixelOffset>" << std::endl
          << "<LineOffset>" << lineOffset << "</LineOffset>";

      vrtfile << std::endl << "</VRTRasterBand>";
    }

    vrtfile << std::endl << "</VRTDataset>" << std::endl;
  }

}


std::string terrama2::core::trim(const std::string& value)
{
  std::string str = value;
  str.erase(std::unique(str.begin(), str.end(),
                        [](char a, char b)
                        { return a == ' ' && b == ' '; }), str.end());

  return QString::fromStdString(str).trimmed().toStdString();
}
