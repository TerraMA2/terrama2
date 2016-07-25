/*  Copyright (C) 2008 National Institute For Space Research (INPE) - Brazil.

    This file is part of the TerraLib - a Framework for building GIS enabled applications.

    TerraLib is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    TerraLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with TerraLib. See COPYING. If not, write to
    TerraLib Team at <terralib-team@terralib.org>.
 */

/*!
  \file terralib/se/serialization/xml/Style.cpp

  \brief Auxiliary classes and functions to serialize style informations from a XML document.
*/

// TerraLib
#include <terralib/core/translator/Translator.h>
#include <terralib/xml/AbstractWriter.h>
#include <terralib/xml/Reader.h>
#include <terralib/se/CoverageStyle.h>
#include <terralib/se/FeatureTypeStyle.h>
#include <terralib/se/Exception.h>
//#include "Description.h"
//#include "Rule.h"
#include <terralib/se/serialization/xml/SymbologySerializer.h>
#include "Style.h"
#include <terralib/se/serialization/xml/Utils.h>



// STL
#include <cassert>

// Boost
#include <boost/format.hpp>

te::se::Style* FeatureTypeStyleReader(te::xml::Reader& reader);
te::se::Style* CoverageStyleReader(te::xml::Reader& reader);

void FeatureTypeStyleWriter(const te::se::Style* style, te::xml::AbstractWriter& writer);
void CoverageStyleWriter(const te::se::Style* layer, te::xml::AbstractWriter& writer);

void te::se::serialize::Style::reg(const std::string& styleType, const StyleFnctSerializeType& fncts)
{
  m_fncts[styleType] = fncts;
}

te::se::Style* te::se::serialize::Style::read(te::xml::Reader& reader) const
{
  std::string styleType = reader.getElementLocalName();

  StyleFnctIdxType::const_iterator it = m_fncts.find(styleType);

  if(it == m_fncts.end())
    throw Exception((boost::format(TE_TR("Could not find a reader for the following style type: %1%.")) % styleType).str());

  assert(it->second.second);

  return it->second.first(reader);
}

void te::se::serialize::Style::write(const te::se::Style* style, te::xml::AbstractWriter& writer) const
{
  assert(style);

  StyleFnctIdxType::const_iterator it = m_fncts.find(style->getType());

  if(it == m_fncts.end())
    throw Exception((boost::format(TE_TR("Could not find a writer for the following style type: %1%.")) % style->getType()).str());

  assert(it->second.second);

  return it->second.second(style, writer);
}

te::se::serialize::Style::~Style()
{
}

te::se::serialize::Style::Style()
{
  m_fncts["FeatureTypeStyle"] = std::make_pair(StyleReadFnctType(&FeatureTypeStyleReader), StyleWriteFnctType(&FeatureTypeStyleWriter));
  m_fncts["CoverageStyle"] = std::make_pair(StyleReadFnctType(&CoverageStyleReader), StyleWriteFnctType(&CoverageStyleWriter));
}

te::se::Style* FeatureTypeStyleReader(te::xml::Reader& reader)
{
  std::auto_ptr<te::se::FeatureTypeStyle> fts(new te::se::FeatureTypeStyle);

  // Version
  if(reader.getNumberOfAttrs() > 0)
  {
    std::string version = reader.getAttr("version");
    fts->setVersion(version);
  }

  reader.next();

  // Name
  if(reader.getElementLocalName() == "Name")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    fts->setName(new std::string(reader.getElementValue()));
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // Description
  if(reader.getElementLocalName() == "Description")
    fts->setDescription(te::se::serialize::ReadDescription(reader));

  // FeatureTypeName
  if(reader.getElementLocalName() == "FeatureTypeName")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    fts->setFeatureTypeName(new std::string(reader.getElementValue()));
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // SemanticTypeIdentifier
  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "SemanticTypeIdentifier")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    fts->push_back(reader.getElementValue());
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // Rules
  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "Rule")
    fts->push_back(te::se::serialize::ReadRule(reader));

  assert(reader.getNodeType() == te::xml::END_DOCUMENT);
  assert(reader.getElementLocalName() == "FeatureTypeStyle");

  reader.next();

  // TODO: OnlineResource

  return fts.release();
}

te::se::Style* CoverageStyleReader(te::xml::Reader& reader)
{
  std::auto_ptr<te::se::CoverageStyle> cs(new te::se::CoverageStyle);

  // Version
  if(reader.getNumberOfAttrs() > 0)
  {
    std::string version = reader.getAttr("version");
    cs->setVersion(version);
  }

  reader.next();

  // Name
  if(reader.getElementLocalName() == "Name")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    cs->setName(new std::string(reader.getElementValue()));
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // Description
  if(reader.getElementLocalName() == "Description")
    cs->setDescription(te::se::serialize::ReadDescription(reader));

  // CoverageName
  if(reader.getElementLocalName() == "CoverageName")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    cs->setCoverageName(new std::string(reader.getElementValue()));
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // SemanticTypeIdentifier
  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "SemanticTypeIdentifier")
  {
    reader.next();
    assert(reader.getNodeType() == te::xml::VALUE);
    cs->push_back(reader.getElementValue());
    reader.next();

    assert(reader.getNodeType() == te::xml::END_ELEMENT);
    reader.next();
  }

  // Rules
  while(reader.getNodeType() == te::xml::START_ELEMENT &&
        reader.getElementLocalName() == "Rule")
    cs->push_back(te::se::serialize::ReadRule(reader));

  assert(reader.getNodeType() == te::xml::END_ELEMENT);
  assert(reader.getElementLocalName() == "CoverageStyle");

  reader.next();

  // TODO: OnlineResource

  return cs.release();
}

void FeatureTypeStyleWriter(const te::se::Style* style, te::xml::AbstractWriter& writer)
{
  const te::se::FeatureTypeStyle* fts = dynamic_cast<const te::se::FeatureTypeStyle*>(style);

  if(fts == 0)
    return;

  writer.writeStartElement("se:FeatureTypeStyle");
  // Version
  writer.writeAttribute("version", fts->getVersion());
  // Namespace
  //writer.writeAttribute("xmlns:se", "http://www.opengis.net/se");
  writer.writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
  // xlink
  //writer.writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

  // Name
  te::se::serialize::WriteStringPtrHelper("se:Name", fts->getName(), writer);
  // Description
  te::se::serialize::Save(fts->getDescription(), writer);
  // FeatureTypeName
  te::se::serialize::WriteStringPtrHelper("se:FeatureTypeName", fts->getFeatureTypeName(), writer);
  // Semantic Type Identifiers
  const std::vector<std::string>* semantics = fts->getSemanticTypeIdentifiers();
  if(semantics != 0)
  {
    for(std::size_t i = 0; i < semantics->size(); ++i)
      writer.writeElement("se:SemanticTypeIdentifier", semantics->at(i));
  }

  // Rules
  std::size_t nRules = fts->getRules().size();

  if(nRules > 0)
  {
    for(std::size_t i = 0; i != nRules; ++i)
      te::se::serialize::Save(fts->getRule(i), writer);
  }
  else // OnlineResources
  {
    assert(fts->getOnlineResources().size() > 0);
    for(std::size_t i = 0; i < fts->getOnlineResources().size(); ++i)
      te::se::serialize::WriteOnlineResourceHelper(fts->getOnlineResource(i), writer);
  }

  writer.writeEndElement("se:FeatureTypeStyle");
}

void CoverageStyleWriter(const te::se::Style* style, te::xml::AbstractWriter& writer)
{
  const te::se::CoverageStyle* cs = dynamic_cast<const te::se::CoverageStyle*>(style);

  if(cs == 0)
    return;

  writer.writeStartElement("se:CoverageStyle");
  // Version
  writer.writeAttribute("version", cs->getVersion());
  // Namespace
  //writer.writeAttribute("xmlns:se", "http://www.opengis.net/se");
  //writer.writeAttribute("xmlns:ogc", "http://www.opengis.net/ogc");
  // xlink
  //writer.writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

  // Name
  te::se::serialize::WriteStringPtrHelper("se:Name", cs->getName(), writer);
  // Description
  te::se::serialize::Save(cs->getDescription(), writer);
  // CoverageName
  te::se::serialize::WriteStringPtrHelper("se:CoverageName", cs->getCoverageName(), writer);
  // Semantic Type Identifiers
  const std::vector<std::string>* semantics = cs->getSemanticTypeIdentifiers();
  if(semantics != 0)
  {
    for(std::size_t i = 0; i < semantics->size(); ++i)
      writer.writeElement("se:SemanticTypeIdentifier", semantics->at(i));
  }

  // Rules
  std::size_t nRules = cs->getRules().size();

  if(nRules > 0)
  {
    for(std::size_t i = 0; i != nRules; ++i)
      te::se::serialize::Save(cs->getRule(i), writer);
  }
  else // OnlineResources
  {
    assert(cs->getOnlineResources().size() > 0);
    for(std::size_t i = 0; i < cs->getOnlineResources().size(); ++i)
      te::se::serialize::WriteOnlineResourceHelper(cs->getOnlineResource(i), writer);
  }

  writer.writeEndElement("se:CoverageStyle");
}
