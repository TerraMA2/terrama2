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
  \file terrama2/services/view/core/serialization/Serialization.cpp

  \brief Serialization methods for View service components

  \author Vinicius Campanha
*/

// TerraMA2
#include "Serialization.hpp"
#include "Exception.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../Utils.hpp"

// TerraLib
#include <terralib/xml/ReaderFactory.h>
#include <terralib/xml/AbstractWriterFactory.h>
#include <terralib/maptools/serialization/xml/Utils.h>
#include <terralib/se/serialization/xml/Style.h>

// Qt
#include <QString>
#include <QObject>

void terrama2::services::view::core::Serialization::writeVectorialStyleGeoserverXML(const te::se::Style* style,
                                                                                    const std::string path)
{

  std::auto_ptr<te::xml::AbstractWriter> writer(te::xml::AbstractWriterFactory::make());

  writer->setURI(path);
  writer->writeStartDocument("UTF-8", "no");

  writer->writeStartElement("StyledLayerDescriptor");

  writer->writeAttribute("xmlns", "http://www.opengis.net/sld");
  writer->writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
  writer->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
  writer->writeAttribute("version", style->getVersion());
  writer->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
  writer->writeAttribute("xsi:schemaLocation", "http://www.opengis.net/sld http://schemas.opengis.net/sld/1.1.0/StyledLayerDescriptor.xsd");
  writer->writeAttribute("xmlns:se", "http://www.opengis.net/se");

  writer->writeStartElement("NamedLayer");
  writer->writeElement("se:Name", "Layer");
  writer->writeStartElement("UserStyle");
  writer->writeElement("se:Name", "Style");

  te::se::serialize::Style::getInstance().write(style, *writer.get());

  writer->writeEndElement("UserStyle");
  writer->writeEndElement("NamedLayer");

  writer->writeEndElement("StyledLayerDescriptor");
  writer->writeToFile();

}


std::unique_ptr<te::se::Style>
terrama2::services::view::core::Serialization::readVectorialStyleXML(const std::string path)
{
  std::unique_ptr<te::se::Style> style;

  std::auto_ptr<te::xml::Reader> reader(te::xml::ReaderFactory::make());
  reader->setValidationScheme(false);

  reader->read(path);
  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "StyledLayerDescriptor"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "NamedLayer"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::VALUE) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::END_ELEMENT) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "UserStyle"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::VALUE) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::END_ELEMENT) ||
      (reader->getElementLocalName() != "Name"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }

  reader->next();

  if ((reader->getNodeType() != te::xml::START_ELEMENT) ||
      (reader->getElementLocalName() != "FeatureTypeStyle"))
  {
    QString errMsg = QObject::tr("Wrong XML format!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw SerializationException() << terrama2::ErrorDescription(errMsg);
  }
  else
  {
    if (reader->getNodeType() == te::xml::START_ELEMENT)
      style.reset(te::se::serialize::Style::getInstance().read(*reader.get()));
  }

  return style;
}

//! Generates OGC ColorMapEntry XML elements with color attributes.
void writeColorMapEntry(te::xml::AbstractWriter* writer,
                        const std::string& color,
                        const std::string& value,
                        const std::string& title,
                        const std::string& opacity)
{
  writer->writeStartElement("ColorMapEntry");
  writer->writeAttribute("color", color);
  writer->writeAttribute("quantity", value);
  writer->writeAttribute("label", title);
  writer->writeAttribute("opacity", opacity);
  writer->writeEndElement("ColorMapEntry");
}

void writeChannelSelection(te::xml::AbstractWriter* writer, const std::string& band)
{
  writer->writeStartElement("ChannelSelection");
  {
    writer->writeStartElement("GrayChannel");
    writer->writeStartElement("SourceChannelName");
    writer->writeValue(band);
    writer->writeEndElement("SourceChannelName");
    writer->writeEndElement("GrayChannel");
  }
  writer->writeEndElement("ChannelSelection");
}

//! Wrapper ColorMapEntry for TerraMA² Rule
void writeColorMapEntry(te::xml::AbstractWriter* writer,
                        const terrama2::services::view::core::View::Legend::Rule& rule)
{
  writeColorMapEntry(writer, rule.color, rule.value, rule.title, rule.opacity);
}

void terrama2::services::view::core::Serialization::writeCoverageStyleGeoserverXML(const View::Legend legend,
                                                                                   const std::string path)
{
  std::unique_ptr<te::xml::AbstractWriter> writer(te::xml::AbstractWriterFactory::make());

  writer->setURI(path);
  writer->writeStartDocument("UTF-8", "no");

  writer->writeStartElement("StyledLayerDescriptor");

  writer->writeAttribute("xmlns", "http://www.opengis.net/sld");
  writer->writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
  writer->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
  writer->writeAttribute("version", "1.0.0");
  writer->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
  writer->writeAttribute("xsi:schemaLocation", "http://www.opengis.net/sld http://schemas.opengis.net/sld/1.0.0/StyledLayerDescriptor.xsd");
  writer->writeAttribute("xmlns:se", "http://www.opengis.net/se");

  writer->writeStartElement("NamedLayer");
  writer->writeElement("Name", "Layer");
  writer->writeStartElement("UserStyle");
  writer->writeElement("Name", "Style");

  writer->writeElement("Title", "Style for a Coverage layer.");

  writer->writeStartElement("FeatureTypeStyle");
  writer->writeStartElement("Rule");

  View::Legend::Rule defaultRule;
  std::vector<View::Legend::Rule> rules;

  for(const auto& rule : legend.rules)
  {
    if(rule.isDefault)
    {
      defaultRule = rule;
      continue;
    }

    rules.push_back(rule);
  }

  // Ordering rules by value asc
  std::sort(rules.begin(), rules.end(), View::Legend::Rule::compareByNumericValue);

  // Retrieving VIEW classification type string
  std::string classifyType = View::Legend::to_string(legend.classify);

  // Band Number as String
  const auto band_number = legend.metadata.find("band_number")->second;

  // Dont create default value for RAMP type
  if(legend.classify != View::Legend::ClassifyType::RAMP)
  {
    // default color
    writer->writeStartElement("RasterSymbolizer");

    // Writing Grey Band channel - Default
    writeChannelSelection(writer.get(), band_number);

    writer->writeStartElement("ColorMap");
    writer->writeAttribute("type", "ramp");

    // Looking for dummy value
    std::unordered_map<std::string, std::string>::const_iterator it = legend.metadata.find("dummy");

    if (it != legend.metadata.end())
    {
      /*
       * #FIXME: Temporary Solution. The GeoServer Styling does not provide a handy way to work both
       * default value (whatever value not mapped) and dummy data (No data in Raster - Transparent).
       * The last implementation tried to use Ramp generation to affect all elements before proceed. But it does
       * not work for Dummy data values that are transparent. In order to skip it, we must define intervals
       * to do not let GeoServer perform color composition. First of all, looking for dummy data.
       * If found, just generate three extra map entries. These entries must contain same values of default
       * rule.
       *
       *  - Entry 1 (-2)
       *  - Entry 2 (-.00000001)
       *  - Dummy Entry
       *  - Entry 3 (+.00000001)
       *  ...
       *
       * The first one value must be "dummy - 2". The second one and the last one value
       * must be nearest zero negative and positive respectively. These values are important to keep GeoServer Color Composition works properly.
       *
       * Note that this approach does not generate GeoServer Legends.
      */

      const double dummy = std::stod(it->second);

      writeColorMapEntry(writer.get(), defaultRule.color, toString(dummy - 2), defaultRule.title, defaultRule.opacity);
      writeColorMapEntry(writer.get(), defaultRule.color, toString(dummy - 0.00000001), defaultRule.title, defaultRule.opacity);

      // Dummy Entry (The color doesn't matter since opacity is 0)
      writeColorMapEntry(writer.get(), defaultRule.color, it->second, "Dummy[No Data]", "0");
      // Continue gradient scale factor for other valeus
      writeColorMapEntry(writer.get(), defaultRule.color, toString(dummy + 0.00000001), defaultRule.title, defaultRule.opacity);
    } // endif it != legend.metadata.end()
    else
    {
      writeColorMapEntry(writer.get(), defaultRule.color, "0", defaultRule.title, defaultRule.opacity);
      writeColorMapEntry(writer.get(), defaultRule.color, "1", defaultRule.title, defaultRule.opacity);
    }

    writer->writeEndElement("ColorMap");
    writer->writeEndElement("RasterSymbolizer");
  }

  // assigned colors
  writer->writeStartElement("RasterSymbolizer");

  // Write Grey Band Channel - Composition
  writeChannelSelection(writer.get(), band_number);

  writer->writeStartElement("ColorMap");

  writer->writeAttribute("type", classifyType);

  for(const auto& rule : rules)
  {
    if(!rule.isDefault)
      writeColorMapEntry(writer.get(), rule);
  }

  writer->writeEndElement("ColorMap");
  writer->writeEndElement("RasterSymbolizer");
  writer->writeEndElement("Rule");
  writer->writeEndElement("FeatureTypeStyle");

  writer->writeEndElement("UserStyle");
  writer->writeEndElement("NamedLayer");

  writer->writeEndElement("StyledLayerDescriptor");
  writer->writeToFile();
}
