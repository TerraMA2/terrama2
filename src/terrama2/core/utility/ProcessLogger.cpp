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
  \file terrama2/core/utility/ProcessLogger.cpp

  \brief

  \author Vinicius Campanha
*/

// TerraMA2
#include "ProcessLogger.hpp"
#include "TimeUtils.hpp"
#include "../Exception.hpp"
#include "../utility/Logger.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype.h>

// Qt
#include <QString>
#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>

// Boost
#include <boost/format.hpp>

terrama2::core::ProcessLogger::ProcessLogger(uint64_t processID, std::map < std::string, std::string > connInfo)
  : processID_(processID)
{
  dataSource_ = te::da::DataSourceFactory::make("POSTGIS");
  dataSource_->setConnectionInfo(connInfo);
  dataSource_->open();

  if(!dataSource_->isOpened())
  {
    QString errMsg = QObject::tr("Could not connect to database");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw LogException() << ErrorDescription(errMsg);
  }
}

terrama2::core::ProcessLogger::~ProcessLogger()
{
  dataSource_->close();
}

void terrama2::core::ProcessLogger::start()
{
  // send start to database

  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("INSERT INTO "+ tableName_ + " (pid, status, start_timestamp, last_process_timestamp) VALUES(%1%, %2%, '%3%', '%4%')");

  query.bind_arg(1, processID_);
  query.bind_arg(2, static_cast<int>(Status::START));
  query.bind_arg(3, TimeUtils::now()->toString());
  query.bind_arg(4, TimeUtils::now()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());

  transactor->commit();

  primaryKey_ = transactor->getLastGeneratedId();
}

void terrama2::core::ProcessLogger::addValue(std::string tag, std::string value)
{
  QString qtag = QString::fromStdString(tag);
  QJsonArray array = obj_[qtag].toArray();
  array.push_back(QString::fromStdString(value));
  obj_.insert(qtag, array);

  updateData();
}

void terrama2::core::ProcessLogger::updateData()
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  QJsonDocument doc(obj_);
  QByteArray json = doc.toJson();

  boost::format query("UPDATE "+ tableName_ + " SET data='%1%' WHERE id =" + QString::number(primaryKey_).toStdString());

  query.bind_arg(1, QString(json).toStdString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}

void terrama2::core::ProcessLogger::error(std::string description)
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("UPDATE "+ tableName_ + " SET status=%1%, error_description='%2%', last_process_timestamp='%3%' WHERE id =" + QString::number(primaryKey_).toStdString());

  query.bind_arg(1, static_cast<int>(Status::ERROR));
  query.bind_arg(2, description);
  query.bind_arg(3, TimeUtils::now()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}

void terrama2::core::ProcessLogger::setTableName(std::string tableName)
{
  tableName_ = tableName;

  if(!dataSource_->dataSetExists(tableName_))
  {
    std::shared_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();

    std::shared_ptr< te::da::DataSetType > datasetType(new te::da::DataSetType(tableName_));

    std::shared_ptr< te::dt::SimpleProperty > id(new te::dt::SimpleProperty("id", te::dt::INT32_TYPE, true));
    id->setAutoNumber(true);
    datasetType->add(id->clone());

    datasetType->add(new te::dt::SimpleProperty("pid", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::SimpleProperty("status", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::DateTimeProperty("start_timestamp", te::dt::TIME_INSTANT_TZ, true));
    datasetType->add(new te::dt::DateTimeProperty("data_timestamp", te::dt::TIME_INSTANT_TZ));
    datasetType->add(new te::dt::DateTimeProperty("last_process_timestamp", te::dt::TIME_INSTANT_TZ, true));
    datasetType->add(new te::dt::StringProperty("error_description", te::dt::STRING));
    datasetType->add(new te::dt::StringProperty("data", te::dt::STRING));

    std::map<std::string, std::string> options;
    transactor->createDataSet(datasetType.get(),options);

    std::shared_ptr<te::dt::Property> id_pk1 = transactor->getProperty(datasetType->getName(),"id");
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(tableName_ + "_pk");
    pk->add(id_pk1.get());

    transactor->addPrimaryKey(datasetType->getName(),pk);

    if(!(transactor->getPrimaryKey(datasetType->getName())).get())
    {
      QString errMsg = QObject::tr("Failed do add primary key to Process Logger table!");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::LogException() << ErrorDescription(errMsg);
    }
  }
}

void terrama2::core::ProcessLogger::done(te::dt::TimeInstantTZ dataTimestamp)
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("UPDATE "+ tableName_ + " SET status=%1%, data_timestamp='%2%', last_process_timestamp='%3%' WHERE id =" + QString::number(primaryKey_).toStdString());

  query.bind_arg(1, static_cast<int>(Status::DONE));
  query.bind_arg(2, dataTimestamp.toString());
  query.bind_arg(3, TimeUtils::now()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getLastProcessTimestamp()
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT MAX(last_process_timestamp) FROM "+ tableName_ + " WHERE pid = " + QString::number(processID_).toStdString();

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::shared_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  if(!tempDataSet)
    return nullptr;

  size_t columnPos = te::da::GetPropertyPos(tempDataSet.get(), "max");

  if(!tempDataSet->moveNext() || tempDataSet->isNull(columnPos))
    return nullptr;

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(columnPos).release()));
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getDataLastTimestamp()
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT MAX(data_timestamp) FROM "+ tableName_ + " WHERE pid = " + QString::number(processID_).toStdString();

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::unique_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  if(!tempDataSet)
    return nullptr;

  size_t columnPos = te::da::GetPropertyPos(tempDataSet.get(), "max");

  if(!tempDataSet->moveNext() || tempDataSet->isNull(columnPos))
    return nullptr;

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(columnPos).release()));
}

uint64_t terrama2::core::ProcessLogger::primaryKey()
{
  return primaryKey_;
}

uint64_t terrama2::core::ProcessLogger::processID()
{
  return processID_;
}
