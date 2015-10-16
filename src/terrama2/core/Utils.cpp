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
  \file terrama2/core/Utils.cpp

  \brief Utility functions for TerraMA2 SDK.

  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Utils.hpp"
#include "../Config.hpp"
#include "Exception.hpp"

// Boost
#include <boost/filesystem.hpp>

// QT
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

std::string terrama2::core::FindInTerraMA2Path(const std::string& p)
{
// 1st: look in the neighborhood of the executable
  boost::filesystem::path tma_path = boost::filesystem::current_path();

  boost::filesystem::path eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  tma_path /= "..";

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

// 2rd: look for an environment variable defined by macro TERRAMA2_DIR_VAR_NAME
  const char* tma_env = getenv(TERRAMA2_DIR_VAR_NAME);

  if(tma_env != nullptr)
  {
    tma_path = tma_env;

    eval_path = tma_path / p;

    if(boost::filesystem::exists(eval_path))
      return eval_path.string();
  }

// 3th: look into install prefix-path
  tma_path = TERRAMA2_INSTALL_PREFIX_PATH;

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

// 4nd: look into the codebase path
  tma_path = TERRAMA2_CODEBASE_PATH;

  eval_path = tma_path / p;

  if(boost::filesystem::exists(eval_path))
    return eval_path.string();

  return "";
}

QJsonDocument
terrama2::core::ReadJsonFile(const std::string &file_name)
{
  QFile file(file_name.c_str());

  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString err_msg(QObject::tr("Could not open file: %1."));
    err_msg = err_msg.arg(file_name.c_str());

    throw terrama2::FileOpenError() << terrama2::ErrorDescription(err_msg);
  }

  QByteArray doc_data = file.readAll();

  file.close();
  
  QJsonParseError parse_error;

  QJsonDocument jdocument = QJsonDocument::fromJson(doc_data, &parse_error);

  if(jdocument.isNull())
  {
    QString err_msg(QObject::tr("Error parsing file '%1': %2."));
    err_msg = err_msg.arg(file_name.c_str()).arg(parse_error.errorString());

    throw terrama2::ParserError() << terrama2::ErrorDescription(err_msg);
  }

  return jdocument;
}

bool
terrama2::core::ToBool(DataProvider::Status status)
{
  switch (status)
  {
    case DataProvider::ACTIVE:
      return true;
    case DataProvider::INACTIVE:
      return false;
    default:
      return false;
  }
}

terrama2::core::DataProvider::Status
terrama2::core::ToDataProviderStatus(bool active)
{
  return (active) ? DataProvider::ACTIVE : DataProvider::INACTIVE;
}

terrama2::core::DataProvider::Kind
terrama2::core::ToDataProviderKind(uint64_t kind)
{
  switch (kind)
  {
    case 1:
      return DataProvider::FTP_TYPE;
    case 2:
      return DataProvider::HTTP_TYPE;
    case 3:
      return DataProvider::FILE_TYPE;
    case 4:
      return DataProvider::WFS_TYPE;
    case 5:
      return DataProvider::WCS_TYPE;
    default:
      return DataProvider::UNKNOWN_TYPE;
  }
}

std::string
terrama2::core::ToString(bool b)
{
  return b ? "true" : "false";
}

bool
terrama2::core::ToBool(DataSet::Status status)
{
  switch (status)
  {
    case DataSet::ACTIVE:
      return true;
    case DataSet::INACTIVE:
      return false;
    default:
      return false;
  }
}


terrama2::core::DataSet::Status terrama2::core::ToDataSetStatus(bool active)
{
  return (active) ? DataSet::ACTIVE : DataSet::INACTIVE;
}



terrama2::core::DataSet::Kind
terrama2::core::ToDataSetKind(uint64_t kind)
{
  switch (kind)
  {
    case 1:
      return DataSet::PCD_TYPE;
    case 2:
      return DataSet::OCCURENCE_TYPE;
    case 3:
      return DataSet::GRID_TYPE;
    default:
      return DataSet::UNKNOWN_TYPE;
  }
}

terrama2::core::DataSetItem::Kind
terrama2::core::ToDataSetItemKind(uint64_t kind)
{
  switch (kind)
  {
    case 1:
      return DataSetItem::PCD_INPE_TYPE;
    case 2:
      return DataSetItem::PCD_TOA5_TYPE;
    case 3:
      return DataSetItem::FIRE_POINTS_TYPE;
    case 4:
      return DataSetItem::DISEASE_OCCURRENCE_TYPE;
    default:
      return DataSetItem::UNKNOWN_TYPE;
  }
}

terrama2::core::DataSetItem::Status
terrama2::core::ToDataSetItemStatus(bool active)
{
  return (active) ? DataSetItem::ACTIVE : DataSetItem::INACTIVE;
}

bool
terrama2::core::ToBool(DataSetItem::Status status)
{
  switch (status)
  {
    case DataSetItem::ACTIVE:
      return true;
    case DataSetItem::INACTIVE:
      return false;
    default:
      return false;
  }
}

terrama2::core::Filter::ExpressionType
terrama2::core::ToFilterExpressionType(uint64_t type)
{
  switch (type)
  {
    case 2:
      return Filter::LESS_THAN_TYPE;
    case 3:
      return Filter::GREATER_THAN_TYPE;
    case 4:
      return Filter::MEAN_LESS_THAN_TYPE;
    case 5:
      return Filter::MEAN_GREATER_THAN_TYPE;
    default:
      return Filter::NONE_TYPE;
  }
}
