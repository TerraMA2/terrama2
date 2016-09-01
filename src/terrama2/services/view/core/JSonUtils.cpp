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
  \file src/terrama2/services/view/core/JSonUtils.cpp

  \brief Methods to convertion between a View and JSon object

  \author Vinicius Campanha
*/

// TerraMA2
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Terralib
#include <terralib/se/Style.h>
#include <terralib/xml/ReaderFactory.h>
#include <terralib/xml/AbstractWriterFactory.h>
#include <terralib/maptools/serialization/xml/Utils.h>
#include <terralib/se/serialization/xml/Style.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QTemporaryFile>

// TODO: Remove this method when find Grouping serialization use
te::map::Grouping* ReadLayerGrouping(te::xml::Reader& reader)
{
  if(reader.getElementLocalName() != "Grouping")
    return 0;

  /* Property Name */
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "PropertyName");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  std::string propertyName = reader.getElementValue();
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  /* Property Data Type */
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "PropertyDataType");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  int propertyType = reader.getElementValueAsInt32();
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  /* Grouping Type */
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Type");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  std::string type = reader.getElementValue();
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  /* Precision */
  reader.next();
  assert(reader.getNodeType() == te::xml::START_ELEMENT);
  assert(reader.getElementLocalName() == "Precision");
  reader.next();
  assert(reader.getNodeType() == te::xml::VALUE);
  std::size_t precision = static_cast<std::size_t>(reader.getElementValueAsInt32());
  reader.next();
  assert(reader.getNodeType() == te::xml::END_ELEMENT);

  std::auto_ptr<te::map::Grouping> g(new te::map::Grouping(propertyName, te::map::serialize::GetGroupingType(type), precision));
  g->setPropertyType(propertyType);

  /* Summary */
  reader.next();
  if(reader.getElementLocalName() == "Summary")
  {
    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    std::string summary = reader.getElementValue();
    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    g->setSummary(summary);
    reader.next();
  }

  if(reader.getElementLocalName() == "StandardDeviation")
  {
    assert(reader.getNodeType() == te::xml::START_ELEMENT);
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    double stdDeviation = reader.getElementValueAsDouble();
    reader.next();
    assert(reader.getNodeType() == te::xml::END_ELEMENT);

    g->setStdDeviation(stdDeviation);

    reader.next();
  }

  /* Grouping Items */
  std::vector<te::map::GroupingItem*> items;
  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "GroupingItem")
  {
    items.push_back(te::map::serialize::ReadGroupingItem(reader));
  }

  assert(reader.getNodeType() == te::xml::END_ELEMENT || reader.getNodeType() == te::xml::END_DOCUMENT);
  reader.next();

  g->setGroupingItems(items);

  return g.release();
}

void writeStyle(const te::se::Style* style, std::string path)
{

  std::auto_ptr<te::xml::AbstractWriter> writer(te::xml::AbstractWriterFactory::make());

  writer->setURI(path);
  writer->writeStartDocument("UTF-8", "no");
  writer->setRootNamespaceURI("http://www.w3.org/2000/xmlns/se");

//  writer->writeStartElement("StyledLayerDescriptor");

//  writer->writeAttribute("xmlns", "http://www.opengis.net/sld");
//  writer->writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
//  writer->writeAttribute("xmlns:se", "http://www.opengis.net/se");
//  writer->writeAttribute("xmlns:se", "http://www.w3.org/2000/xmlns/se");
//  writer->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
//  writer->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
//  writer->writeAttribute("xsi:schemaLocation", "http://www.opengis.net/sld StyledLayerDescriptor.xsd");

//  writer->writeAttribute("version", style->getVersion());

//  writer->writeStartElement("NamedLayer");
//  writer->writeStartElement("UserStyle");

  te::se::serialize::Style::getInstance().write(style, *writer.get());

//  writer->writeEndElement("UserStyle");
//  writer->writeEndElement("NamedLayer");

//  writer->writeEndElement("StyledLayerDescriptor");
  writer->writeToFile();

}


QJsonObject terrama2::services::view::core::toJson(ViewPtr view)
{
  QJsonObject obj;
  obj.insert("class", QString("View"));
  obj.insert("id", static_cast<int32_t>(view->id));
  obj.insert("project_id", static_cast<int32_t>(view->projectId));
  obj.insert("service_instance_id", static_cast<int32_t>(view->serviceInstanceId));
  obj.insert("active", view->active);
  obj.insert("imageName", QString(view->imageName.c_str()));
  obj.insert("imageType", static_cast<int32_t>(view->imageType));
  obj.insert("imageResolutionWidth", static_cast<int32_t>(view->imageResolutionWidth));
  obj.insert("imageResolutionHeight", static_cast<int32_t>(view->imageResolutionHeight));
  obj.insert("schedule", terrama2::core::toJson(view->schedule));
  obj.insert("srid", static_cast<int32_t>(view->srid));

  {
    QJsonArray array;
    for(auto& it : view->dataSeriesList)
    {
      QJsonObject datasetSeries;
      datasetSeries.insert("dataset_series_id", static_cast<int32_t>(it));
      array.push_back(datasetSeries);
    }
    obj.insert("data_series_list", array);
  }

  {
    QJsonArray array;
    for(auto& it : view->filtersPerDataSeries)
    {
      QJsonObject datasetSeriesAndFilter;
      datasetSeriesAndFilter.insert("dataset_series_id", static_cast<int32_t>(it.first));
      datasetSeriesAndFilter.insert("dataset_series_filter", terrama2::core::toJson(it.second));
      array.push_back(datasetSeriesAndFilter);
    }
    obj.insert("filters_per_data_series", array);
  }

  // Style serialization
  {
    QJsonArray array;
    for(auto& it : view->stylesPerDataSeries)
    {
      QJsonObject datasetSeriesAndStyle;
      datasetSeriesAndStyle.insert("dataset_series_id", static_cast<int32_t>(it.first));

      QTemporaryFile file;
      if(!file.open())
        throw Exception() << ErrorDescription("Could not create XML file!");

      writeStyle(it.second->clone(), file.fileName().toStdString());

      QByteArray content = file.readAll();
      if(content.isEmpty())
      {
        QString errMsg = QObject::tr("Could not create XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      datasetSeriesAndStyle.insert("dataset_series_view_style", QString(content));

      array.push_back(datasetSeriesAndStyle);
    }
    obj.insert("styles_per_data_series", array);
  }

  // Grouping Serialization
  {
    QJsonArray array;
    for(auto& it : view->legendPerDataSeries)
    {
      QJsonObject datasetSeriesAndLegend;
      datasetSeriesAndLegend.insert("dataset_series_id", static_cast<int32_t>(it.first));

      std::unique_ptr<te::xml::AbstractWriter> writer(te::xml::AbstractWriterFactory::make());

      QTemporaryFile file;
      if(!file.open())
        throw Exception() << ErrorDescription("Could not create XML file!");

      writer->setURI(file.fileName().toStdString());
      te::map::serialize::WriteLayerGrouping(new te::map::Grouping(*it.second), *writer.get());

      writer->writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
      writer->writeAttribute("xmlns:se", "http://www.opengis.net/se");

      writer->writeToFile();

      QByteArray content = file.readAll();
      if(content.isEmpty())
      {
        QString errMsg = QObject::tr("Could not create XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      datasetSeriesAndLegend.insert("dataset_series_view_legend", QString(content));

      array.push_back(datasetSeriesAndLegend);
    }
    obj.insert("legends_per_data_series", array);
  }

  return obj;
}


std::unique_ptr<te::se::Style> readStyle(std::string path)
{
  std::unique_ptr<te::se::Style> style;

  std::auto_ptr<te::xml::Reader> reader(te::xml::ReaderFactory::make());
  reader->setValidationScheme(false);

  reader->read(path);
  reader->next();

  if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
      (reader->getElementLocalName() == "StyledLayerDescriptor"))
    reader->next();

  if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
      (reader->getElementLocalName() == "NamedLayer"))
    reader->next();

  if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
      (reader->getElementLocalName() == "UserStyle"))
    reader->next();

  if ((reader->getNodeType() == te::xml::START_ELEMENT) &&
      (reader->getElementLocalName() == "FeatureTypeStyle"))
  {
    if (reader->getNodeType() == te::xml::START_ELEMENT)
      style.reset(te::se::serialize::Style::getInstance().read(*reader.get()));
  }

  return style;
}

terrama2::services::view::core::ViewPtr terrama2::services::view::core::fromViewJson(QJsonObject json)
{
  if(json["class"].toString() != "View")
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!json.contains("id")
     || !json.contains("project_id")
     || !json.contains("service_instance_id")
     || !json.contains("active")
     || !json.contains("imageName")
     || !json.contains("imageType")
     || !json.contains("imageResolutionWidth")
     || !json.contains("imageResolutionHeight")
     || !json.contains("schedule")
     || !json.contains("srid")
     || !json.contains("data_series_list")
     || !json.contains("filters_per_data_series")
     || !json.contains("styles_per_data_series")
     || !json.contains("legends_per_data_series"))
  {
    QString errMsg = QObject::tr("Invalid View JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
  terrama2::services::view::core::ViewPtr viewPtr(view);

  view->id = static_cast<uint32_t>(json["id"].toInt());
  view->projectId = static_cast<uint32_t>(json["project_id"].toInt());
  view->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());
  view->active = json["active"].toBool();
  view->imageName = json["imageName"].toString().toStdString();
  view->imageType = te::map::ImageType(json["imageType"].toInt());
  view->imageResolutionWidth = static_cast<uint32_t>(json["imageResolutionWidth"].toInt());
  view->imageResolutionHeight = static_cast<uint32_t>(json["imageResolutionHeight"].toInt());
  view->srid = static_cast<uint32_t>(json["srid"].toInt());

  view->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());

  {
    auto datasetSeriesArray = json["data_series_list"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();
      view->dataSeriesList.push_back(static_cast<uint32_t>(obj["dataset_series_id"].toInt()));
    }
  }

  {
    auto datasetSeriesArray = json["filters_per_data_series"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();
      view->filtersPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()), terrama2::core::fromFilterJson(json["dataset_series_filter"].toObject()));
    }
  }

  // Read Style
  {
    auto datasetSeriesArray = json["styles_per_data_series"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();

      QTemporaryFile file;

      if(!file.open())
      {
        QString errMsg = QObject::tr("Could not load the XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      file.write(obj["dataset_series_view_style"].toString().toUtf8());
      file.flush();

      std::unique_ptr<te::se::Style> style = readStyle(file.fileName().toStdString());

      if(!style)
      {
        QString errMsg = QObject::tr("Could not read the XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      view->stylesPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()),
          std::unique_ptr<te::se::Style>(style.release()));
    }
  }

  {
    auto datasetSeriesArray = json["legends_per_data_series"].toArray();
    auto it = datasetSeriesArray.begin();
    for(; it != datasetSeriesArray.end(); ++it)
    {
      auto obj = (*it).toObject();

      QTemporaryFile file;

      if(!file.open())
      {
        QString errMsg = QObject::tr("Could not load the XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      file.write(obj["dataset_series_view_legend"].toString().toUtf8());
      file.flush();

      std::unique_ptr<te::xml::Reader> reader(te::xml::ReaderFactory::make());
      reader->setValidationScheme(false);

      reader->read(file.fileName().toStdString());

      if(!reader->next())
      {
        QString errMsg = QObject::tr("Could not read the XML file!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      // TODO: Remove when Terralib fix Grouping serialization
      view->legendPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()),
          std::unique_ptr<te::map::Grouping>(ReadLayerGrouping(*reader.get())));

      // TODO: Enable when Terralib fix Grouping serialization
      //      view->legendPerDataSeries.emplace(static_cast<uint32_t>(obj["dataset_series_id"].toInt()),
      //          std::unique_ptr<te::map::Grouping>(te::map::serialize::ReadLayerGrouping(*reader.get())));
    }
  }

  return viewPtr;
}
