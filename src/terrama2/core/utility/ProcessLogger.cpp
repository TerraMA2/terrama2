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

terrama2::core::ProcessLogger::ProcessLogger(const uint64_t processID, const std::map < std::string, std::string > connInfo)
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

uint64_t terrama2::core::ProcessLogger::start()
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

  return transactor->getLastGeneratedId();
}

void terrama2::core::ProcessLogger::addValue(const std::string tag, const std::string value, uint64_t registerId)
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT data FROM "+ tableName_ + " WHERE id = " + QString::number(registerId).toStdString();

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::shared_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  if(!tempDataSet)
  {
    QString errMsg = QObject::tr("Can not find log message table name!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  if(!tempDataSet->moveNext())
  {
    QString errMsg = QObject::tr("Error to access log message table!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  QByteArray readJson = tempDataSet->getAsString("data").c_str();

  QJsonDocument docJson(QJsonDocument::fromJson(readJson));
  QJsonObject obj = docJson.object();
  QString qtag = QString::fromStdString(tag);
  QJsonArray array = obj[qtag].toArray();
  array.push_back(QString::fromStdString(value));
  obj.insert(qtag, array);

  updateData(registerId, obj);
}


void terrama2::core::ProcessLogger::error(const std::string description, uint64_t registerId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::shared_ptr< te::dt::TimeInstantTZ > now(TimeUtils::now());

  boost::format query("UPDATE "+ tableName_ + " SET status=%1%, last_process_timestamp='%2%' WHERE id =" + QString::number(registerId).toStdString());

  query.bind_arg(1, static_cast<int>(Status::ERROR));
  query.bind_arg(2, now->toString());

  boost::format queryMessages("INSERT INTO messages_" + QString::number(processID_).toStdString() + "(log_id, type, description, timestamp) VALUES(" + QString::number(registerId).toStdString() + ", %1%, '%2%', '%3%')");
  queryMessages.bind_arg(1, static_cast<int>(messageType::ERROR_MESSAGE));
  queryMessages.bind_arg(2, description);
  queryMessages.bind_arg(3, now->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->execute(queryMessages.str());
  transactor->commit();
}


void terrama2::core::ProcessLogger::done(const std::shared_ptr<te::dt::TimeInstantTZ> dataTimestamp, uint64_t registerId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("UPDATE "+ tableName_ + " SET status=%1%, data_timestamp='%2%', last_process_timestamp='%3%' WHERE id =" + QString::number(registerId).toStdString());

  query.bind_arg(1, static_cast<int>(Status::DONE));
  query.bind_arg(2, dataTimestamp->toString());
  query.bind_arg(3, TimeUtils::now()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getLastProcessTimestamp() const
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

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getDataLastTimestamp() const
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

uint64_t terrama2::core::ProcessLogger::processID() const
{
  return processID_;
}

void terrama2::core::ProcessLogger::setTableName(const std::string tablePrefixName, const uint64_t instanceID)
{
  tableName_ = tablePrefixName + std::to_string(instanceID);

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

  messagesTableName_ = "messages_" + std::to_string(instanceID);

  if(!dataSource_->dataSetExists(messagesTableName_))
  {
    std::shared_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();

    std::shared_ptr< te::da::DataSetType > datasetType(new te::da::DataSetType(messagesTableName_));

    std::shared_ptr< te::dt::SimpleProperty > id(new te::dt::SimpleProperty("id", te::dt::INT32_TYPE, true));
    id->setAutoNumber(true);
    datasetType->add(id->clone());

    datasetType->add(new te::dt::SimpleProperty("log_id", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::SimpleProperty("type", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::StringProperty("description", te::dt::STRING));
    datasetType->add(new te::dt::DateTimeProperty("timestamp", te::dt::TIME_INSTANT_TZ, true));

    std::map<std::string, std::string> options;
    transactor->createDataSet(datasetType.get(),options);

    std::shared_ptr<te::dt::Property> id_pk1 = transactor->getProperty(datasetType->getName(),"id");
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(messagesTableName_ + "_pk");
    pk->add(id_pk1.get());

    transactor->addPrimaryKey(datasetType->getName(),pk);

    std::shared_ptr<te::dt::Property> log_id = transactor->getProperty(datasetType->getName(),"log_id");
    te::da::ForeignKey* fk = new te::da::ForeignKey(messagesTableName_ + "_fk");

    std::shared_ptr<te::dt::Property> id_fk = transactor->getProperty(tableName_,"id");
    fk->addRefProperty(id_fk.get());
    fk->setReferencedDataSetType(dataSource_->getDataSetType(tableName_).release());

    fk->add(log_id.get());

    transactor->addForeignKey(datasetType->getName(),fk);

    if(!(transactor->getPrimaryKey(datasetType->getName())).get())
    {
      QString errMsg = QObject::tr("Failed do add primary key to Process Logger table!");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::LogException() << ErrorDescription(errMsg);
    }
  }
}

void terrama2::core::ProcessLogger::updateData(uint64_t registerId, QJsonObject obj)
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  QJsonDocument doc(obj);
  QByteArray json = doc.toJson();

  boost::format query("UPDATE "+ tableName_ + " SET data='%1%' WHERE id =" + QString::number(registerId).toStdString());

  query.bind_arg(1, QString(json).toStdString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}
