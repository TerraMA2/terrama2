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
#include "../core/utility/Verify.hpp"

//TerraLib
#include <terralib/datatype/DateTimeProperty.h>
#include <terralib/datatype/SimpleProperty.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/memory/Raster.h>
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

//Boost
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

//STL
#include <unordered_set>
#include <boost/filesystem.hpp>

terrama2::core::DataAccessorGrADS::DataAccessorGrADS(DataProviderPtr dataProvider, DataSeriesPtr dataSeries,
                                                     const bool checkSemantics)
  : DataAccessor(dataProvider, dataSeries, false),
    DataAccessorGDAL(dataProvider, dataSeries, false)
{
  if(checkSemantics && dataSeries->semantics.code != dataAccessorType())
  {
    QString errMsg = QObject::tr("Wrong DataSeries semantics.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw WrongDataSeriesSemanticsException() << ErrorDescription(errMsg);
  }
}


std::string terrama2::core::DataAccessorGrADS::getControlFileMask(DataSetPtr dataSet) const
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

std::string terrama2::core::DataAccessorGrADS::retrieveData(const DataRetrieverPtr dataRetriever,
                                                            DataSetPtr dataset,
                                                            const Filter& filter,
                                                            std::shared_ptr<FileRemover> remover) const
{
  std::string folderPath = "";

  try
  {
    folderPath = getFolderMask(dataset, dataSeries_);
  }
  catch(UndefinedTagException& /*e*/)
  {
    // Do nothing
  }

  std::string mask = getControlFileMask(dataset);
  std::string uri = dataRetriever->retrieveData(mask, filter, remover, "", folderPath);

  QUrl url(QString::fromStdString(uri+"/"+folderPath));
  QDir dir(url.path());
  auto fileList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
  for(const auto& file : fileList)
  {
    gradsDescriptor_ = readDataDescriptor(url.path().toStdString()+"/"+file.toStdString());

    std::string datasetMask = gradsDescriptor_.datasetFilename_;
    if(gradsDescriptor_.datasetFilename_[0] == '^')
    {
      gradsDescriptor_.datasetFilename_.erase(0, 1);
      datasetMask = gradsDescriptor_.datasetFilename_;
    }

    datasetMask = grad2TerramaMask(datasetMask.c_str()).toStdString();

    // In case the user specified a binary file mask, use it instead of the one in the CTL file.
    try
    {
      std::string binaryFileMask = getMask(dataset);
      if(!binaryFileMask.empty())
      {
        datasetMask = binaryFileMask;
      }
    }
    catch(...)
    {
      // In case no binary file mask specified, use dataset mask in the CTL file.
    }

    dataRetriever->retrieveData(datasetMask, filter, remover, uri, folderPath);
  }

  return uri;
}

std::string terrama2::core::DataAccessorGrADS::dataSourceType() const
{
  return "GDAL";
}

void terrama2::core::DataAccessorGrADS::addToCompleteDataSet(DataSetPtr dataSet,
                                                             std::shared_ptr<te::mem::DataSet> completeDataSet,
                                                             std::shared_ptr<te::da::DataSet> teDataSet,
                                                             std::shared_ptr<te::dt::TimeInstantTZ> fileTimestamp,
                                                             const std::string& filename) const
{
  completeDataSet->moveLast();

  size_t rasterColumn = te::da::GetFirstPropertyPos(teDataSet.get(), te::dt::RASTER_TYPE);
  if(!isValidColumn(rasterColumn))
  {
    QString errMsg = QObject::tr("No raster attribute.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  size_t timestampColumn = te::da::GetFirstPropertyPos(completeDataSet.get(), te::dt::DATETIME_TYPE);

  teDataSet->moveBeforeFirst();
  while(teDataSet->moveNext())
  {
    std::unique_ptr<te::rst::Raster> raster(teDataSet->isNull(rasterColumn) ? nullptr : teDataSet->getRaster(rasterColumn).release());


    // if the pixel data is in the reverse order, invert.
    invertRaster(raster);

    // multiply the raster by a factor
    checkRasterMultiply(dataSet, raster);

    te::mem::DataSetItem* item = new te::mem::DataSetItem(completeDataSet.get());
    item->setRaster(rasterColumn, raster.release());
    if(isValidColumn(timestampColumn))
      item->setDateTime(timestampColumn,
                        fileTimestamp.get() ? static_cast<te::dt::DateTime*>(fileTimestamp->clone()) : nullptr);

    item->setString("filename", filename);

    completeDataSet->add(item);
  }
}

void terrama2::core::DataAccessorGrADS::checkRasterMultiply(terrama2::core::DataSetPtr dataSet, std::unique_ptr<te::rst::Raster>& raster) const
{
  try
  {
    // multiply every pixel by a given factor
    auto multiplier = getValueMultiplier(dataSet);
    if(multiplier != 1)
    {
      raster = terrama2::core::multiplyRaster(*raster, multiplier);
    }
  }
  catch(const UndefinedTagException&)
  {
    //nothing to do
  }
}

void terrama2::core::DataAccessorGrADS::invertRaster(std::unique_ptr<te::rst::Raster>& raster) const
{
  if(!yReverse_)
    return;

  std::vector<te::rst::BandProperty*> bands;
  for(size_t i = 0; i < raster->getNumberOfBands(); ++i)
  {
    bands.push_back(new te::rst::BandProperty(*raster->getBand(i)->getProperty()));
  }
  auto grid = new te::rst::Grid(raster->getNumberOfColumns(), raster->getNumberOfRows(), new te::gm::Envelope(*raster->getExtent()), raster->getSRID());
  std::unique_ptr<te::rst::Raster> expansible(te::rst::RasterFactory::make("EXPANSIBLE", grid, bands, {}));

  auto rows = grid->getNumberOfRows();
  for(uint band = 0; band < raster->getNumberOfBands(); ++band)
  {
    for(uint row = 0; row < rows; ++row)
    {
      for(uint col = 0; col < grid->getNumberOfColumns(); ++col)
      {
        std::complex<double> value;
        raster->getValue(col, rows-row-1, value, band);
        expansible->setValue(col, row, value, band);
      }
    }
  }

  raster = std::move(expansible);
}

QString terrama2::core::DataAccessorGrADS::grad2TerramaMask(QString mask) const
{

  /*
   * GrADS date format
    %y2  - YY    2 digit year
    %y4  - YYYY  4 digit year
    %m2  - MM    2 digit month (leading zero if needed)
    %d2  - DD    2 digit day (leading zero if needed)
    %h2  - hh    2 digit hour
    %n2  - mm    2 digit minute; leading zero if needed
  */

  mask = mask.replace("%y2", "%YY");
  mask = mask.replace("%y4", "%YYYY");
  mask = mask.replace("%m2", "%MM");
  mask = mask.replace("%d2", "%DD");
  mask = mask.replace("%h2", "%hh");
  mask = mask.replace("%n2", "%mm");

  return mask;
}

bool terrama2::core::DataAccessorGrADS::needToOpenConfigFile() const
{
  return true;
}


bool terrama2::core::DataAccessorGrADS::hasControlFile() const
{
  return true;
}

std::string terrama2::core::DataAccessorGrADS::getConfigFilename(terrama2::core::DataSetPtr dataSet, const std::string& binaryFilename) const
{

  std::string extension = boost::filesystem::extension(binaryFilename);
  std::string vrtFilename = binaryFilename;
  boost::replace_last(vrtFilename, extension, ".vrt");

  writeVRTFile(gradsDescriptor_, binaryFilename, vrtFilename, dataSet);

  return vrtFilename;
}

std::string terrama2::core::DataAccessorGrADS::readControlFile(terrama2::core::DataSetPtr dataSet, const std::string& controlFilename) const
{
  gradsDescriptor_ = readDataDescriptor(controlFilename);
  gradsDescriptor_.srid_ = getSrid(dataSet);

  std::string datasetMask = gradsDescriptor_.datasetFilename_;
  if(gradsDescriptor_.datasetFilename_[0] == '^')
  {
    gradsDescriptor_.datasetFilename_.erase(0, 1);
    datasetMask = gradsDescriptor_.datasetFilename_;
  }

  datasetMask = grad2TerramaMask(datasetMask.c_str()).toStdString();

  // In case the user specified a binary file mask, use it instead of the one in the CTL file.
  try
  {
    std::string binaryFileMask = getMask(dataSet);
    if(!binaryFileMask.empty())
    {
      datasetMask = binaryFileMask;
    }
  }
  catch(...)
  {
    // In case no binary file mask specified, use dataset mask in the CTL file.
  }

  return datasetMask;
}


void terrama2::core::GrADSDataDescriptor::addVar(const std::string& strVar)
{
  Var* var = new Var;
  QStringList tokens = QString::fromStdString(strVar).split(" ");

  if(tokens.size() >= 4)
  {
    var->varName_ = tokens[0].toStdString();

    bool ok = true;
    var->verticalLevels_ = tokens[1].toInt(&ok);
    if(!ok)
    {
      QString errMsg = QObject::tr("Invalid value for VAR, expected an INT and found: %1").arg(tokens[1]);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataAccessorException() << ErrorDescription(errMsg);
    }

    var->units_ = tokens[2].toStdString();

    // Description may have spaces, need to concatenate all pieces
    for(int i = 3; i < tokens.size(); ++i)
    {
      if(!var->description_.empty())
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
  if(key == "DSET")
  {
    datasetFilename_ = value;
    found = true;
  }
  else if(key == "TITLE")
  {
    title_ = value;
    found = true;
  }
  else if(key == "UNDEF")
  {
    undef_ = std::stod(value, nullptr);
    found = true;
  }
  else if(key.find("OPTIONS") != std::string::npos)
  {
    QStringList tokens = QString::fromStdString(value).split(" ");
    for(QString token : tokens)
    {
      vecOptions_.push_back(token.toUpper().toStdString());
    }

    found = true;
  }
  else if(key == "XDEF")
  {
    xDef_ = getValueDef(value, "X");
    if(xDef_ != nullptr)
      found = true;
  }
  else if(key == "YDEF")
  {
    yDef_ = getValueDef(value, "Y");
    if(yDef_ != nullptr)
      found = true;
  }
  else if(key == "ZDEF")
  {
    zDef_ = getValueDef(value, "Z");
    if(zDef_ != nullptr)
      found = true;
  }
  else if(key == "TDEF")
  {
    tDef_ = getTValueDef(value);
    if(tDef_ != nullptr)
      found = true;
  }
  //TODO: Implement rest of the parameters

  if(!found)
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
  if(tokens.size() < 3)
  {
    delete valueDef;
    QString errMsg = QObject::tr("Wrong number of fields in TDEF configuration, expected at least 3");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  bool ok = true;
  valueDef->numValues_ = tokens[0].toInt(&ok);
  if(!ok)
  {
    QString errMsg = QObject::tr("Invalid value for TDEF, expected an INT and found: %1").arg(tokens[0]);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  if(tokens[1].toUpper() == "LINEAR")
    valueDef->dimensionType_ = LINEAR;
  else if(tokens[1].toUpper() == "LEVELS")
    valueDef->dimensionType_ = LEVELS;
  else
  {
    delete valueDef;
    QString errMsg = QObject::tr("Invalid mapping method for TDEF");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  for(int i = 2; i < tokens.size(); ++i)
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
  if(tokens.size() < 3)
  {
    delete valueDef;
    QString errMsg = QObject::tr("Wrong number of fields in %1DEF configuration, expected at least 3").arg(
                       dimension.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  bool ok = true;
  valueDef->numValues_ = tokens[0].toInt();
  if(!ok)
  {
    QString errMsg = QObject::tr("Invalid value for %1DEF, expected an INT and found: %2").arg(dimension.c_str()).arg(tokens[1]);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  std::string token = tokens[1].toUpper().toStdString();
  if(token == "LINEAR")
    valueDef->dimensionType_ = LINEAR;
  else if(token == "LEVELS")
    valueDef->dimensionType_ = LEVELS;
  else if(token == "GAUST62" && dimension
          == "Y")
    valueDef->dimensionType_ = GAUST62;
  else if(token == "GAUSR15" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR15;
  else if(token == "GAUSR20" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR20;
  else if(token == "GAUSR30" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR30;
  else if(token == "GAUSR40" && dimension == "Y")
    valueDef->dimensionType_ = GAUSR40;
  else
  {
    delete valueDef;
    QString errMsg = QObject::tr("Invalid mapping method for %1DEF").arg(dimension.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataAccessorException() << ErrorDescription(errMsg);
  }

  for(int i = 2; i < tokens.size(); ++i)
  {
    bool ok = true;
    double value = tokens[i].toDouble(&ok);

    if(!ok)
    {
      QString errMsg = QObject::tr("Invalid value for %1DEF, expected a double and found: %2").arg(dimension.c_str()).arg(tokens[i]);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataAccessorException() << ErrorDescription(errMsg);
    }
    valueDef->values_.push_back(value);
  }

  return valueDef;
}

terrama2::core::GrADSDataDescriptor& terrama2::core::GrADSDataDescriptor::operator=(const GrADSDataDescriptor& rhs)
{
  if(this == &rhs)
    return *this;

  // Delete pointers from own copy
  for(size_t i = 0; i < vecCHSUB_.size(); ++i)
    delete vecCHSUB_[i];
  vecCHSUB_.clear();

  if(xDef_ != nullptr)
  {
    delete xDef_;
    xDef_ = nullptr;
  }

  if(yDef_ != nullptr)
  {
    delete yDef_;
    yDef_ = nullptr;
  }

  if(zDef_ != nullptr)
  {
    delete zDef_;
    zDef_ = nullptr;
  }

  if(tDef_ != nullptr)
  {
    delete tDef_;
    zDef_ = nullptr;
  }

  for(size_t i = 0; i < vecVars_.size(); ++i)
    delete vecVars_[i];
  vecVars_.clear();

  //copy values from rhs
  datasetFilename_ = rhs.datasetFilename_;
  title_ = rhs.title_;
  vecOptions_ = rhs.vecOptions_;
  undef_ = rhs.undef_;
  fileHeaderLength_ = rhs.fileHeaderLength_;
  srid_ = rhs.srid_;

  for(size_t i = 0; i < rhs.vecCHSUB_.size(); ++i)
    vecCHSUB_.push_back(new CHSUB(*rhs.vecCHSUB_[i]));

  if(rhs.xDef_ != nullptr)
    xDef_ = new ValueDef(*rhs.xDef_);

  if(rhs.yDef_ != nullptr)
    yDef_ = new ValueDef(*rhs.yDef_);

  if(rhs.zDef_ != nullptr)
    zDef_ = new ValueDef(*rhs.zDef_);

  if(rhs.tDef_ != nullptr)
    tDef_ = new TValueDef(*rhs.tDef_);

  for(size_t i = 0; i < rhs.vecVars_.size(); ++i)
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

  for(size_t i = 0; i < rhs.vecCHSUB_.size(); ++i)
    vecCHSUB_.push_back(new CHSUB(*rhs.vecCHSUB_[i]));

  if(rhs.xDef_ != nullptr)
    xDef_ = new ValueDef(*rhs.xDef_);

  if(rhs.yDef_ != nullptr)
    yDef_ = new ValueDef(*rhs.yDef_);

  if(rhs.zDef_ != nullptr)
    zDef_ = new ValueDef(*rhs.zDef_);

  if(rhs.tDef_ != nullptr)
    tDef_ = new TValueDef(*rhs.tDef_);

  for(size_t i = 0; i < rhs.vecVars_.size(); ++i)
    vecVars_.push_back(new Var(*rhs.vecVars_[i]));
}

terrama2::core::GrADSDataDescriptor::~GrADSDataDescriptor()
{
  for(size_t i = 0; i < vecCHSUB_.size(); ++i)
    delete vecCHSUB_[i];

  if(xDef_ != nullptr)
    delete xDef_;

  if(yDef_ != nullptr)
    delete yDef_;

  if(zDef_ != nullptr)
    delete zDef_;

  if(tDef_ != nullptr)
    delete tDef_;

  for(size_t i = 0; i < vecVars_.size(); ++i)
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
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
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
  while(!in.atEnd())
  {
    switch(state)
    {
      case FindSection:
      {
        line = trim(in.readLine().toStdString());
        if(line.empty() || line.front() == '*')
          continue;

        std::string key(line);
        std::string value;
        std::size_t found = line.find(" ");
        if(found != std::string::npos)
        {
          key = line.substr(0, found);
          value = line.substr(found + 1);
        }

        boost::to_upper(key);

        if(key == "VARS")
        {
          section = "VARS";
          state = FindVar;
        }
        else if(key == "ENDVARS")
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
        if(line.empty())
        {
          state = FindSection;
          continue;
        }

        std::string key(line);
        std::string value;
        std::size_t found = line.find(" ");
        if(found != std::string::npos)
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

        if(line.empty())
          continue;

        if(line == "ENDVARS")
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
                                                     const std::string& vrtFilename,
                                                     DataSetPtr dataset) const
{

  try
  {
    uint32_t numBands = getNumberOfBands(dataset);
    if(numBands > 0)
      descriptor.numberOfBands_ = numBands;
    else
      descriptor.numberOfBands_ = (uint32_t)descriptor.tDef_->numValues_;
  }
  catch(UndefinedTagException e)
  {
    // In case the didn't specify the number of bands it will use the number of bands in the file

  }

  try
  {
    descriptor.valueMultiplier_ = getValueMultiplier(dataset);
  }
  catch(UndefinedTagException e)
  {
    // In case the didn't specify the number of bands it will use the number of bands in the file
  }

  std::ofstream vrtfile;
  vrtfile.open(vrtFilename);

  if(!vrtfile.is_open())
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

    terrama2::core::verify::srid(descriptor.srid_);

    const std::string wktStr = te::srs::SpatialReferenceSystemManager::getInstance().getWkt(descriptor.srid_);

    if(wktStr.empty())
    {
      QString errMsg = QObject::tr("Empty WKT for srid: %1.").arg(descriptor.srid_);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataAccessorException() << ErrorDescription(errMsg);
    }

    vrtfile << std::endl << "<SRS>" << wktStr << "</SRS>";

    // The yRev option from the grads consider the DATA from north to south, that's our normal orientation
    // if yRev is not set the raster lines will be inverted
    yReverse_ = ! (std::find(descriptor.vecOptions_.begin(), descriptor.vecOptions_.end(), "YREV") != descriptor.vecOptions_.end());

    //FIXME: don't work if the image area stars before the 180 degree line and ends after.
    //ticket: https://trac.dpi.inpe.br/terrama2/ticket/935

    //change longitude from 0/360 to -180/180
    if(descriptor.xDef_->values_[0] > 180)
      descriptor.xDef_->values_[0] = descriptor.xDef_->values_[0] - 360;

    if((descriptor.xDef_->values_[1] == 0.0) || (descriptor.yDef_->values_[1] == 0.0))
    {
      QString errMsg = QObject::tr("Invalid resolution in dataset: %1").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw DataAccessorException() << ErrorDescription(errMsg);
    }

    vrtfile << std::endl << "<GeoTransform>" << descriptor.xDef_->values_[0] << ","
            << descriptor.xDef_->values_[1] << ",0," << descriptor.yDef_->values_[0] + descriptor.yDef_->numValues_ * descriptor.yDef_->values_[1] << ",0,"
            << -descriptor.yDef_->values_[1]
            << "</GeoTransform>";

    bool isSequential = std::find(descriptor.vecOptions_.begin(), descriptor.vecOptions_.end(), "SEQUENTIAL") != descriptor.vecOptions_.end();

    unsigned int dataTypeSizeBytes = 4; // Float32
    std::string dataType = "Float32";
    try
    {
      std::string dataTypeStr = getDataType(dataset);
      if(dataTypeStr == "INT16")
      {
        dataTypeSizeBytes = 2;
        dataType = "Int16";
      }
    }
    catch(...)
    {
    }


    unsigned int pixelOffset = dataTypeSizeBytes;

    if(!isSequential)
      pixelOffset *= descriptor.numberOfBands_;

    unsigned int lineOffset = pixelOffset * descriptor.xDef_->numValues_;
    unsigned int bytesAfter = getBytesAfter(dataset);
    unsigned int bytesBefore = getBytesBefore(dataset);
    unsigned int imageOffset = 0;

    for(uint32_t bandIdx = 0; bandIdx < descriptor.numberOfBands_; ++bandIdx)
    {
      imageOffset += bytesBefore;

      vrtfile
          << std::endl
          << "<VRTRasterBand dataType=\"" + dataType + "\" band=\"" << (bandIdx + 1) << "\" subClass=\"VRTRawRasterBand\">"
          << std::endl
          << "<SourceFilename relativetoVRT=\"1\">" << binFilename << "</SourceFilename>" << std::endl
          << "<ImageOffset>" << imageOffset << "</ImageOffset>" << std::endl
          << "<PixelOffset>" << pixelOffset << "</PixelOffset>" << std::endl
          << "<LineOffset>" << lineOffset << "</LineOffset>" << std::endl
          << "<NoDataValue>" << descriptor.undef_ << "</NoDataValue>" << std::endl;
      vrtfile << std::endl << "</VRTRasterBand>";


      if(isSequential)
      {
        imageOffset += lineOffset * descriptor.yDef_->numValues_;
      }
      else
      {
        imageOffset += dataTypeSizeBytes;
      }

      imageOffset += bytesAfter;

    }

    vrtfile << std::endl << "</VRTDataset>" << std::endl;
  }

}

uint32_t terrama2::core::DataAccessorGrADS::getBytesBefore(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return (uint32_t)std::stoi(dataset->format.at("bytes_before"), nullptr);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for bytes before in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

uint32_t terrama2::core::DataAccessorGrADS::getBytesAfter(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return (uint32_t)std::stoi(dataset->format.at("bytes_after"), nullptr);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for bytes after in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

uint32_t terrama2::core::DataAccessorGrADS::getNumberOfBands(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return (uint32_t)std::stoi(dataset->format.at("number_of_bands"), nullptr);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for number of bands in dataset: %1.").arg(dataset->id);
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}


double terrama2::core::DataAccessorGrADS::getValueMultiplier(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return std::stod(dataset->format.at("value_multiplier"), nullptr);
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for number of bands in dataset: %1.").arg(dataset->id);
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorGrADS::getDataType(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return dataset->format.at("data_type");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for data type in dataset: %1.").arg(dataset->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorGrADS::getMask(terrama2::core::DataSetPtr dataset) const
{
  try
  {
    return dataset->format.at("binary_file_mask");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined tag for binary file mask in dataset: %1.").arg(dataset->id);
    throw UndefinedTagException() << ErrorDescription(errMsg);
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
