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
  \file terrama2/analysis/core/Analysis.cpp

  \brief Model class for the analysis configuration.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Analysis.hpp"
#include "Utils.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/Exception.hpp"

// QT
#include <QObject>


void terrama2::analysis::core::Analysis::setId(uint64_t id)
{
	id_ = id;
}

uint64_t terrama2::analysis::core::Analysis::id() const
{
	return id_;
}

void terrama2::analysis::core::Analysis::setAdditionalDataList(const std::vector<terrama2::core::DataSeriesPtr>& additionalDataList)
{
  additionalDataList_ = additionalDataList;
}

std::vector<terrama2::core::DataSeriesPtr> terrama2::analysis::core::Analysis::additionalDataList() const
{
  return additionalDataList_;
}

void terrama2::analysis::core::Analysis::setMonitoredObject(terrama2::core::DataSeriesPtr monitoredObject)
{
	monitoredObject_ = monitoredObject;
}

terrama2::core::DataSeriesPtr terrama2::analysis::core::Analysis::monitoredObject() const
{
  return monitoredObject_;
}

void terrama2::analysis::core::Analysis::setScriptLanguage(const ScriptLanguage scriptLanguage)
{
	scriptLanguage_ = scriptLanguage;
}

void terrama2::analysis::core::Analysis::setDCP(terrama2::core::DataSeriesPtr pcd, Influence influence)
{
  pcd_ = pcd;
  mapInfluence_[pcd->id] = influence;
}

terrama2::core::DataSeriesPtr terrama2::analysis::core::Analysis::DCP() const
{
  return pcd_;
}

terrama2::analysis::core::Analysis::ScriptLanguage terrama2::analysis::core::Analysis::scriptLanguage() const
{
	return scriptLanguage_;
}

void terrama2::analysis::core::Analysis::setScript(const std::string& script)
{
	script_ = script;
}

std::string terrama2::analysis::core::Analysis::script() const
{
	return script_;
}

void terrama2::analysis::core::Analysis::setDescription(const std::string& description)
{
	description_ = description;
}

std::string terrama2::analysis::core::Analysis::description() const
{
	return description_;
}

void terrama2::analysis::core::Analysis::setType(const Type type)
{
	type_ = type;
}

terrama2::analysis::core::Analysis::Type terrama2::analysis::core::Analysis::type() const
{
	return type_;
}

terrama2::analysis::core::Analysis::Influence terrama2::analysis::core::Analysis::influence(DataSeriesId dataSeriesId) const
{
  auto it = mapInfluence_.find(dataSeriesId);

  if(it == mapInfluence_.end())
    throw terrama2::InvalidArgumentException() << ErrorDescription(QObject::tr("Could not find an influence configuration for this dataset."));
  return it->second;
}

void terrama2::analysis::core::Analysis::setInfluence(uint64_t datasetId, Influence influence)
{
  mapInfluence_[datasetId] = influence;
}

