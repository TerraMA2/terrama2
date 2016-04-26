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
#include <terralib/dataaccess/dataset/DataSet.h>

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

  boost::format query("INSERT INTO "+ tableName_ + " (PID, status, process_timestamp) VALUES(%1%, %2%, '%3%')");

  query.bind_arg(1, processID_);
  query.bind_arg(2, static_cast<int>(Status::START));
  query.bind_arg(3, TimeUtils::now()->toString());

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

  boost::format query("INSERT INTO "+ tableName_ + " (data) VALUES('%1%') WHERE ID =" + QString::number(primaryKey_).toStdString());

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

  boost::format query("INSERT INTO "+ tableName_ + " (status, error_description, process_timestamp) VALUES(%1%, '%2%', '%3%') WHERE ID =" + QString::number(primaryKey_).toStdString());

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
}

void terrama2::core::ProcessLogger::done(te::dt::TimeInstantTZ dataTimestamp)
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("INSERT INTO "+ tableName_ + " (status, data_timestamp, process_timestamp) VALUES(%1%, '%2%', '%3%') WHERE ID =" + QString::number(primaryKey_).toStdString());

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

  std::string sql = "SELECT process_timestamp FROM "+ tableName_ + " WHERE ID = " + QString::number(primaryKey_).toStdString();

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::unique_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(0).release()));
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getDataTimestamp()
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT MAX(data_timestamp) FROM "+ tableName_ + " WHERE PID = " + QString::number(processID_).toStdString();

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::unique_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(0).release()));
}

uint64_t terrama2::core::ProcessLogger::primaryKey()
{
  return primaryKey_;
}

uint64_t terrama2::core::ProcessLogger::processID()
{
  return processID_;
}
