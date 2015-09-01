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
  \file terrama2/gui/config/RemoteWeatherServerData.hpp

  \brief Definition of methods in class RemoteWeatherServerData.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


#ifndef _REMOTESERVERSDATA_H_
#define _REMOTESERVERSDATA_H_

// QT
#include <QString>
#include <QList>

#include "soapModAnaliseProxy.h"

//! Classe responsável por representar uma configuração de dado do servidor
class RemoteWeatherServerData
{
public:
  RemoteWeatherServerData();
  RemoteWeatherServerData(const wsWeatherDataSource& data);
  ~RemoteWeatherServerData();

  //! Retorna o identificador do dado do servidor
  int id() const { return _data.id; }

  void setId(int id);  

  //! Retorna true se for um formato de dado novo ainda não salvo na base de dados
  bool isNew() const { return _data.id == -1; }

  //! Retorna a geomtria do dado
  enum wsWeatherDataSourceGeometry geometry() const { return _data.geometry; }

  //! Altera a geometria do dado
  void setGeometry(enum wsWeatherDataSourceGeometry   geometry) { _data.geometry = geometry; }

  //! Retorna o nome do dado
  QString name() const { return QString::fromStdString(_data.name); }

  //! Altera o nome do dado
  void setName(QString name) { _data.name = name.toStdString(); }

  //! Retorna o tipo do dado
  enum wsWeatherDataSourceType type() const { return _data.type; }

  //! Altera o tipo do dado
  void setType(enum wsWeatherDataSourceType type) { _data.type = type; }

  //! Retorna a resolucao espacial
  double spatialResolution() const { return _data.spatialResolution; }

  //! Altera a resolucao espacial
  void setSpatialResolution(double spatialResolution) { _data.spatialResolution = spatialResolution; }

  //! Retorna a frequencia de atualizacao em minutos
  double updateFrequencyMins() const { return _data.updateFrequencyMins; }

  //! Altera a frequencia de atualizacao em minutos
  void setUpdateFrequencyMins(double updateFrequencyMins) { _data.updateFrequencyMins = updateFrequencyMins; }

  //! Retorna detalhes do dado
  QString details() const { return QString::fromStdString(_data.details); }

  //! Altera detalhes do dado
  void setDetails(QString details) { _data.details = details.toStdString(); }

  //! Retorna o caminho do dado
  QString path() const { return QString::fromStdString(_data.path); }

  //! Altera o caminho do dado
  void setPath(QString path) { _data.path = path.toStdString(); }

  //! Retorna a mascara para o nome do arquivo
  QString mask() const { return QString::fromStdString(_data.mask); }

  //! Altera a mascara para o nome do arquivo
  void setMask(QString mask) { _data.mask = mask.toStdString(); }
  
  //! Retorna o tipo da area de influência da PCD
  enum wsWeatherDataPcdInfluenceType pcd_influenceType() const { return _data.pcd_influenceType; }
  
  //! Altera o tipo de area de influencia de uma PCD
  void setPcd_influenceType(enum wsWeatherDataPcdInfluenceType mode) { _data.pcd_influenceType = mode; }

  //! Retorna o Id do tema associado com a PCD
	int pcd_influenceThemeId() const { return _data.pcd_influenceThemeId; }
	
	//! Altera o tema associado com uma PCD
	void setPcd_influenceThemeId(int id) { _data.pcd_influenceThemeId = id; }
	
	//! Retorna o nome do atributo de ligação entre o tema e as PCDs
	QString pcd_influenceAttribute() const { return QString::fromStdString(_data.pcd_influenceAttribute); }
	
	//! Altera o nome do atributo de ligação entre o tema e as PCDs
	void setPcd_influenceAttribute(QString attr) { _data.pcd_influenceAttribute = attr.toStdString(); }

  //! Retorna o formato padrao do dado
  enum wsWeatherDataSourceFileFormat format() const { return _data.format; }

  //! Altera o formato padrao do dado
  void setFormat(enum wsWeatherDataSourceFileFormat format) { _data.format = format; }

  //! Altera os dados de filtro
  void setFilter(const wsWeatherDataSourceFilter& filterParams) { _data.filters = filterParams; }

  //! Retorna a estrutura que empacota os dados de filtro
  const wsWeatherDataSourceFilter& filter() { return _data.filters; }

  //! Altera a configuração do pré-processamento sobre dados pontuais em diferentes localizações
  void setIntersection(const wsWeatherDataSourceIntersection& interParams) { _data.intersection = interParams; }

  //! Retorna a estrutura que empacota a configuração do pré-processamento sobre dados pontuais em diferentes localizações
  const wsWeatherDataSourceIntersection& intersection() { return _data.intersection; }

  //! Altera os dados de projeção
  void setProjection(const wsProjectionParams& projection) { _data.projection = projection; }

  //! Retorna a estrutura que empacota os dados de projeção
  const wsProjectionParams& projection() { return _data.projection; }

  //! Retorna a estrutura que empacota os dados de análise
  const wsWeatherDataSource& data() const { return _data; }

  //----------------------------------------------------------
  // Dados associados ao tipo de formato de dados
  //----------------------------------------------------------
  //! Retorna a unidade das coordenadas (AscIIGrid)
  enum wsASCIIGridCoordUnit asciiGrid_coordUnit() const { return _data.asciiGrid_coordUnit; }

  //! Altera a unidade das coordenadas (AscIIGrid)
  void setAsciiGrid_coordUnit(enum wsASCIIGridCoordUnit asciiGridCoordUnit) { _data.asciiGrid_coordUnit = asciiGridCoordUnit; }

  //! Retorna a colecao de pontos (PCD)
  std::vector<struct wsPCD> pcd_collectionPoints() const { return _data.pcd_collectionPoints; }

  //! Altera a colecao de pontos (PCD)
  void setPcd_collectionPoints(std::vector<struct wsPCD> pcdColPts) { _data.pcd_collectionPoints = pcdColPts; }

  //! Retorna a regra de coleta de pontos (PCD)
  QString pcd_collectionRuleLuaScript() const { return QString::fromStdString(_data.pcd_collectionRuleLuaScript); }

  //! Altera a regra de coleta de pontos (PCD)
  void setPcd_collectionRuleLuaScript(const QString & pcdColRule) { _data.pcd_collectionRuleLuaScript = pcdColRule.toStdString(); }

  //! Retorna a colecao de atributos (PCD)
  std::vector<std::string> pcd_collectionAtributes() const { return _data.pcd_attributes; }

  //! Altera a colecao de atributos (PCD)
  void setPcd_collectionAtributes(std::vector<std::string> pcdAtributes) { _data.pcd_attributes = pcdAtributes; }

  //! Retorna a colecao de surfaces (PCD)
  std::vector<struct wsWeatherDataPcdSurface> pcd_collectionSurfaces() const { return _data.pcd_surfaces; }

  //! Altera a colecao de surfaces criadas (PCD)
  void setPcd_collectionSurfaces(std::vector<struct wsWeatherDataPcdSurface> pcdSurfaces) { _data.pcd_surfaces = pcdSurfaces; }

  //! Retorna o nome do atributo que representa o identificador da PCD em um layer WFS (PCD OGC WFS)
  QString wfs_pcd_id_attr() const { return QString::fromStdString(_data.wfs_pcd_id_attr); }

  //! Altera o nome do atributo que representa o identificador da PCD em um layer WFS (PCD OGC WFS)
  void setWFS_pcd_id_attr(const QString & pcdIdAttr) { _data.wfs_pcd_id_attr = pcdIdAttr.toStdString(); }

  //! Retorna o nome do atributo que representa o timestamp das coletas de uma PCD em um layer WFS (PCD OGC WFS)
  QString wfs_pcd_timestamp_attr() const { return QString::fromStdString(_data.wfs_pcd_timestamp_attr); }

  //! Altera o nome do atributo que representa o timestamp das coletas de uma PCD em um layer WFS (PCD OGC WFS)
  void setWFS_pcd_timestamp_attr(const QString & pcdTimestampAttr) { _data.wfs_pcd_timestamp_attr = pcdTimestampAttr.toStdString(); }

  //! Retorna a mascara usada para extrair a data/hora do atributo temporal de um layer WFS (PCD OGC WFS)
  QString wfs_pcd_timestamp_mask() const { return QString::fromStdString(_data.wfs_pcd_timestamp_mask); }

  //! Altera a mascara usada para extrair a data/hora do atributo temporal de um layer WFS (PCD OGC WFS)
  void setWFS_pcd_timestamp_mask(const QString & pcdTimestampMask) { _data.wfs_pcd_timestamp_mask = pcdTimestampMask.toStdString(); }

  //! Retorna o nome do arquivo de navegação (TIFF)
  QString tiff_navigationFile() const { return QString::fromStdString(_data.tiff_navigationFile); }

  //! Altera o nome do arquivo de navegação (TIFF)
  void setTiff_navigationFile(QString tiffNavFile) { _data.tiff_navigationFile = tiffNavFile.toStdString(); }

  //! Retorna o nome do arquivo de controle (Grads)
  QString grads_ctlFile() const { return QString::fromStdString(_data.grads_ctlFile); }

  //! Altera o nome do arquivo de controle (Grads)
  void setGrads_ctlFile(QString gradsCtlFile) { _data.grads_ctlFile = gradsCtlFile.toStdString(); }

  //! Retorna o tipo de dado (Grads)
  enum wsGrADSDataType grads_dataType() const { return _data.grads_dataType; }

  //! Altera o tipo de dado (Grads)
  void setGrads_dataType(enum wsGrADSDataType gradsDtType) { _data.grads_dataType = gradsDtType; }

  //! Retorna o multiplicador (Grads)
  double grads_multiplier() const { return _data.grads_multiplier; }

  //! Altera o multiplicador (Grads)
  void setGrads_multiplier(double gradsMult) { _data.grads_multiplier = gradsMult; }

  //! Retorna identificação de como os bytes estão ordenados na memória
  enum wsGrADSSwapType grads_swap() const { return _data.grads_swap; }
  
  //! Altera identificação de como os bytes estão ordenados na memória
  void setGrads_swap(enum wsGrADSSwapType swap) { _data.grads_swap = swap; }
  
  //! Retorna o número de bandas presentes no arquivo
  int grads_numBands() const { return _data.grads_numBands; }
  
  //! Altera o número de bandas presentes no arquivo
  void setGrads_numBands(int nbands) { _data.grads_numBands = nbands; }
  
  //! Retorna o tamanho em bytes do header antes da matriz de dados de cada banda
  int grads_bandHeader() const { return _data.grads_bandHeader; }
  
  //! Altera o tamanho em bytes do header antes da matriz de dados de cada banda
  void setGrads_bandHeader(int size) { _data.grads_bandHeader = size; }
  
  //! Retorna o tamanho em bytes do header antes da matriz de dados de cada banda
  int grads_bandTrailler() const { return _data.grads_bandTrailler; }
  
  //! Altera o tamanho em bytes do header antes da matriz de dados de cada banda
  void setGrads_bandTrailler(int size) { _data.grads_bandTrailler = size; }

  //! Retorna o numero de minutos entre uma banda e outra
  int grads_bandTimeOffset() const { return _data.grads_bandTimeOffset; }
  
  //! Altera o numero de minutos entre uma banda e outra
  void setGrads_bandTimeOffset(int offset) { _data.grads_bandTimeOffset = offset; }

  //! Retorna o valor dummy do raster da fonte de dados (OGC WCS)
  double wcs_dummy() const { return _data.wcs_dummy; }

  //! Altera o valor dummy do raster da fonte de dados (OGC WCS)
  void setWCS_dummy(double dummy) { _data.wcs_dummy = dummy; };

  //! Retorna o valor do identificador auxiliar
  int auxiliar_id() const { return _data.auxiliar_id; }

  //! Altera o valor do identificador auxiliar
  void setAuxiliar_id(int auxiliar) { _data.auxiliar_id = auxiliar; };

  //! Retorna o timezone da fonte de dados
  QString timeZone() { return QString::fromStdString(_data.timezone); } 
  
  //! Altera o timezone da fonte de dados
  void setTimeZone(QString timezone) { _data.timezone = timezone.toStdString(); }

  //! Retorna o prefixo utilizado pela fonte de dados
  QString prefix() { return QString::fromStdString(_data.prefix); }
  
  //! Altera o prefixo utiliziado pela fonte de dados
  void setPrefix(QString& prefix) { _data.prefix = prefix.toStdString(); }

  //! Retorna a unidade da fonte de dados
  QString unit() { return QString::fromStdString(_data.unit); }

  //! Altera a unidade da fonte de dados
  void setUnit(QString unit) { _data.unit = unit.toStdString(); }

	//----------------------------------------------------------

private:
  wsWeatherDataSource _data;  //! Dados dos formatos de dado empacotados na estrutura lida
};


#endif

