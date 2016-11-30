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
#include "../utility/Logger.hpp"
#include "../utility/Verify.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype.h>
#include <terralib/core/uri/URI.h>

// Qt
#include <QString>
#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>

// Boost
#include <boost/format.hpp>

// STL
#include <utility>

void terrama2::core::ProcessLogger::setConnectionInfo(const te::core::URI& uri) noexcept
{
  try
  {
    closeConnection();

    dataSource_ = te::da::DataSourceFactory::make("POSTGIS", uri);

    try
    {
      dataSource_->open();

      if(!dataSource_->isOpened())
      {
        QString errMsg = QObject::tr("Could not connect to database");
        TERRAMA2_LOG_ERROR() << errMsg;
      }
    }
    catch(std::exception& e)
    {
      QString errMsg = QObject::tr("Could not connect to database");
      TERRAMA2_LOG_ERROR() << errMsg << ": " << e.what();
    }
  }
  catch(std::exception& e)
  {
    QString errMsg = QObject::tr("Could not connect to database");
    TERRAMA2_LOG_ERROR() << errMsg << ": " << e.what();
  }
  catch(...)
  {
    // exception guard, slots should never emit exceptions.
    TERRAMA2_LOG_ERROR() << QObject::tr("Unknown exception...");
  }

}

void terrama2::core::ProcessLogger::setDataSource(te::da::DataSource* dataSource)
{
  dataSource_.reset(dataSource);

  try
  {
    dataSource_->open();

    if(!dataSource_->isOpened())
    {
      throw LogException();
    }
  }
  catch(std::exception& e)
  {
    QString errMsg = QObject::tr("Could not connect to database");
    TERRAMA2_LOG_ERROR() << errMsg << ": " << e.what();
    throw LogException() << ErrorDescription(errMsg);
  }
}

void terrama2::core::ProcessLogger::closeConnection()
{
  if(dataSource_ && dataSource_->isOpened())
    dataSource_->close();
}

terrama2::core::ProcessLogger::~ProcessLogger()
{
  closeConnection();
}


RegisterId terrama2::core::ProcessLogger::start(ProcessId processId) const
{
  // send start to database

  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("INSERT INTO "+ tableName_ + " (process_id, status, start_timestamp, last_process_timestamp) VALUES(%1%, %2%, '%3%', '%4%')");

  query.bind_arg(1, processId);
  query.bind_arg(2, static_cast<int>(Status::START));
  query.bind_arg(3, TimeUtils::nowUTC()->toString());
  query.bind_arg(4, TimeUtils::nowUTC()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());

  transactor->commit();

  return transactor->getLastGeneratedId();
}

void terrama2::core::ProcessLogger::addValue(const std::string& tag, const std::string& value, RegisterId registerId) const
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


void
terrama2::core::ProcessLogger::log(MessageType messageType, const std::string &description, RegisterId registerId) const
{
  if(tableName_.empty() || messagesTableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log tables names.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::shared_ptr< te::dt::TimeInstantTZ> now(TimeUtils::nowUTC());

  std::string escapedDescription(description);
  // TODO: Remove it when terralib escape work properly
  std::replace(escapedDescription.begin(), escapedDescription.end(), '\'', ' ');

  boost::format queryMessages("INSERT INTO " + messagesTableName_ + " (log_id, type, description, timestamp) VALUES(" + QString::number(registerId).toStdString() + ", %1%, '%2%', '%3%')");
  queryMessages.bind_arg(1, static_cast<int>(messageType));
  queryMessages.bind_arg(2, escapedDescription);
  queryMessages.bind_arg(3, now->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(transactor->escape(queryMessages.str()));

  transactor->commit();
}


void terrama2::core::ProcessLogger::result(Status status, const std::shared_ptr<te::dt::TimeInstantTZ> &dataTimestamp,
                                           RegisterId registerId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  boost::format query("UPDATE "+ tableName_ + " SET status=%1%, data_timestamp=%2%, last_process_timestamp='%3%' WHERE id =" + QString::number(registerId).toStdString());
  QString timestamp = "NULL";


  if(dataTimestamp != nullptr)
  {
    verify::date(dataTimestamp);

    auto boostTime = dataTimestamp->getTimeInstantTZ();
    timestamp = QString::fromStdString(dataTimestamp->toString());

    timestamp.prepend("'");
    timestamp.append("'");
  }


  query.bind_arg(1, static_cast<int>(status));
  query.bind_arg(2, timestamp);
  query.bind_arg(3, TimeUtils::nowUTC()->toString());

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();
  transactor->execute(query.str());
  transactor->commit();
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getLastProcessTimestamp(const ProcessId processId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT MAX(last_process_timestamp) FROM "+ tableName_ + " WHERE process_id = " + std::to_string(processId);

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::shared_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  if(!tempDataSet)
    return nullptr;

  size_t columnPos = te::da::GetPropertyPos(tempDataSet.get(), "max");

  if(!tempDataSet->moveNext() || tempDataSet->isNull(columnPos))
    return nullptr;

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(columnPos).release()));
}

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::ProcessLogger::getDataLastTimestamp(const ProcessId processId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT MAX(data_timestamp) FROM "+ tableName_ + " WHERE process_id = " + std::to_string(processId);

  std::shared_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::unique_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  if(!tempDataSet)
    return nullptr;

  size_t columnPos = te::da::GetPropertyPos(tempDataSet.get(), "max");

  if(!tempDataSet->moveNext() || tempDataSet->isNull(columnPos))
    return nullptr;

  return std::shared_ptr< te::dt::TimeInstantTZ >(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime(columnPos).release()));
}

std::vector< terrama2::core::ProcessLogger::Log > terrama2::core::ProcessLogger::getLogs(const ProcessId processId, uint32_t begin, uint32_t end) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  if(begin > end)
    std::swap(begin, end);

  int rowNumbers = (end - begin) + 1;

  std::string sql ="SELECT * FROM " + tableName_ +
                   " WHERE process_id = "  + std::to_string(processId) +
                   " ORDER BY id DESC" +
                   " LIMIT " + std::to_string(rowNumbers) +
                   " OFFSET " + std::to_string(begin);

  std::unique_ptr< te::da::DataSourceTransactor > transactor = dataSource_->getTransactor();

  std::unique_ptr<te::da::DataSet> tempDataSet(transactor->query(sql));

  std::vector< Log > logs;

  while(tempDataSet->moveNext())
  {
    Log tempLog;

    tempLog.id = tempDataSet->getInt32("id");
    tempLog.processId = tempDataSet->getInt32("process_id");
    tempLog.status = Status(tempDataSet->getInt32("status"));
    tempLog.start_timestamp = std::shared_ptr<te::dt::TimeInstantTZ>(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime("start_timestamp").release()));
    tempLog.data_timestamp = std::shared_ptr<te::dt::TimeInstantTZ>(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime("data_timestamp").release()));
    tempLog.last_process_timestamp = std::shared_ptr<te::dt::TimeInstantTZ>(dynamic_cast<te::dt::TimeInstantTZ*>(tempDataSet->getDateTime("last_process_timestamp").release()));
    tempLog.data = tempDataSet->getAsString("data");


    std::string sqlMessages ="SELECT * FROM " + messagesTableName_ +
                             " WHERE log_id = "  + std::to_string(tempLog.id) +
                             "ORDER BY id";

    std::unique_ptr<te::da::DataSet> tempMessagesDataSet(transactor->query(sqlMessages));

    std::vector< MessageLog > messages;

    while(tempMessagesDataSet->moveNext())
    {
      MessageLog tempMessage;

      tempMessage.id = tempMessagesDataSet->getInt32("id");
      tempMessage.log_id = RegisterId(tempMessagesDataSet->getInt32("log_id"));
      tempMessage.type = MessageType(tempMessagesDataSet->getInt32("type"));
      tempMessage.description = tempMessagesDataSet->getAsString("description");
      tempMessage.timestamp = std::shared_ptr<te::dt::TimeInstantTZ>(dynamic_cast<te::dt::TimeInstantTZ*>(tempMessagesDataSet->getDateTime("timestamp").release()));

      messages.push_back(tempMessage);
    }

    tempLog.messages = messages;

    logs.push_back(tempLog);
  }

  return logs;
}

ProcessId terrama2::core::ProcessLogger::processID(const RegisterId registerId) const
{
  if(tableName_.empty())
  {
    QString errMsg = QObject::tr("Can not find log table name. Is it setted?");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::LogException() << ErrorDescription(errMsg);
  }

  std::string sql = "SELECT process_id FROM "+ tableName_ + " WHERE id = " + QString::number(registerId).toStdString();

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

  return tempDataSet->getInt32("process_id");
}

void terrama2::core::ProcessLogger::setTableName(std::string& tableName)
{
  // Check if schema_ exists in database
  {
    std::shared_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();
    transactor->execute("CREATE SCHEMA IF NOT EXISTS " + schema_);

    transactor->commit();
  }

  std::transform(tableName.begin(), tableName.end(), tableName.begin(), ::tolower);

  tableName_ = schema_ + "." + tableName;

  if(!dataSource_->dataSetExists(tableName_))
  {
    std::shared_ptr<te::da::DataSourceTransactor> transactor = dataSource_->getTransactor();

    std::shared_ptr< te::da::DataSetType > datasetType(new te::da::DataSetType(tableName_));

    std::shared_ptr< te::dt::SimpleProperty > id(new te::dt::SimpleProperty("id", te::dt::INT32_TYPE, true));
    id->setAutoNumber(true);
    datasetType->add(id->clone());

    datasetType->add(new te::dt::SimpleProperty("process_id", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::SimpleProperty("status", te::dt::INT32_TYPE, true));
    datasetType->add(new te::dt::DateTimeProperty("start_timestamp", te::dt::TIME_INSTANT_TZ, true));
    datasetType->add(new te::dt::DateTimeProperty("data_timestamp", te::dt::TIME_INSTANT_TZ));
    datasetType->add(new te::dt::DateTimeProperty("last_process_timestamp", te::dt::TIME_INSTANT_TZ, true));
    datasetType->add(new te::dt::StringProperty("data", te::dt::STRING));

    std::map<std::string, std::string> options;
    transactor->createDataSet(datasetType.get(),options);

    std::shared_ptr<te::dt::Property> id_pk1 = transactor->getProperty(datasetType->getName(),"id");
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(tableName + "_pk");
    pk->add(id_pk1.get());

    transactor->addPrimaryKey(datasetType->getName(),pk);

    if(!(transactor->getPrimaryKey(datasetType->getName())).get())
    {
      QString errMsg = QObject::tr("Failed do add primary key to Process Logger table!");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::LogException() << ErrorDescription(errMsg);
    }

    transactor->commit();
  }

  messagesTableName_ = tableName_ + "_messages";

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
    te::da::PrimaryKey* pk = new te::da::PrimaryKey(tableName + "_messages_pk");
    pk->add(id_pk1.get());

    transactor->addPrimaryKey(datasetType->getName(),pk);

    std::shared_ptr<te::dt::Property> log_id = transactor->getProperty(datasetType->getName(),"log_id");
    te::da::ForeignKey* fk = new te::da::ForeignKey(tableName + "_fk");

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

    transactor->commit();
  }
}

void terrama2::core::ProcessLogger::updateData(const ProcessId registerId, const QJsonObject obj) const
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
