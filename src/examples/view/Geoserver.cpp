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
  \file examples/view/GeoServer.cpp

  \brief

  \author Vinicius Campanha
 */

// Qt
#include <QTemporaryFile>

// TerraMA2
#include "Utils.hpp"

#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/services/view/core/data-access/Geoserver.hpp>
#include <terrama2/services/view/core/data-access/DataAccess.hpp>
#include <terrama2/services/view/core/JSonUtils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>


int main(int argc, char** argv)
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();

  try
  {

    // Make sure to have a geoServer with the below configuration
    te::core::URI uri("http://admin:geoserver@localhost:8080/geoserver");
    terrama2::services::view::core::GeoServer geoserver(uri);

    geoserver.registerWorkspace("aworkspace");

    geoserver.registerStyle("arealStyle", "<?xml version=\"1.0\" encoding=\"UTF-8\"?><sld:StyledLayerDescriptor xmlns=\"http://www.opengis.net/sld\" xmlns:sld=\"http://www.opengis.net/sld\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:ogc=\"http://www.opengis.net/ogc\" version=\"1.0.0\"><sld:NamedLayer><sld:Name>Style</sld:Name><sld:UserStyle><sld:Name>Style</sld:Name><sld:FeatureTypeStyle><sld:Name>Style 1</sld:Name><sld:Rule><sld:RasterSymbolizer><sld:Opacity>1.0</sld:Opacity><sld:ColorMap extended=\"true\"><sld:ColorMapEntry color=\"#000000\" quantity=\"0.0\"/><sld:ColorMapEntry color=\"#ffffff\" quantity=\"1.0\"/></sld:ColorMap></sld:RasterSymbolizer></sld:Rule></sld:FeatureTypeStyle></sld:UserStyle></sld:NamedLayer></sld:StyledLayerDescriptor>");

    // Registering shapes from the same server that GeoServer
    geoserver.registerVectorFile("ashape", TERRAMA2_DATA_DIR + "/shapefile/Rod_Principais_SP_lin.shp", "shp");

    geoserver.registerVectorFile("ashape", TERRAMA2_DATA_DIR + "/shapefile/35MUE250GC_SIR.shp", "shp");

    // Removing the shapes
    geoserver.deleteVectorLayer("ashape", "Rod_Principais_SP_lin", true);

    geoserver.deleteVectorLayer("ashape", "35MUE250GC_SIR.shp", true);

    // Uploading many shapes from a zip file
    geoserver.uploadZipVectorFiles("ashapes", TERRAMA2_DATA_DIR + "/shapefile/shapefile.zip", "shp");

    // Removing the shapes
    geoserver.deleteVectorLayer("ashapes", "Rod_Principais_SP_lin", true);

    geoserver.deleteVectorLayer("ashapes", "35MUE250GC_SIR.shp", true);

    // Registering a folder with shapes in the GeoServer
    geoserver.registerVectorsFolder("ashapesfolder", TERRAMA2_DATA_DIR + "/shapefile", "shp");

    geoserver.deleteVectorLayer("ashapesfolder", "35MUE250GC_SIR.shp", true);

    // Publish a table in Postgis with vector data
    std::map<std::string, std::string> connInfo { {"PG_HOST", TERRAMA2_DATABASE_HOST},
                                                  {"PG_PORT", TERRAMA2_DATABASE_PORT},
                                                  {"PG_USER", TERRAMA2_DATABASE_USERNAME},
                                                  {"PG_PASSWORD", TERRAMA2_DATABASE_PASSWORD},
                                                  {"PG_DB_NAME", TERRAMA2_DATABASE_DBNAME},
                                                  {"PG_CONNECT_TIMEOUT", "4"},
                                                  {"PG_CLIENT_ENCODING", "UTF-8"}
                                                };

    std::unique_ptr<te::da::DataSetType> dsType(terrama2::services::view::core::DataAccess::getDataSetType("dataSourceURI",
                                                               "muni", "POSTGIS"));

    geoserver.registerPostgisTable("ashapepostgis", connInfo, "muni", "muni", dsType);

    geoserver.registerPostgisTable("aviewpostgis", connInfo, "view_muni", "view_muni", dsType, "", "SELECT * FROM muni WHERE gid = 558");

    // Registering a style
    geoserver.registerStyle("astyle", "style");

    // Registering coverages from the same server that GeoServer
    geoserver.registerCoverageFile("acoverage", TERRAMA2_DATA_DIR + "/geotiff/Spot_Vegetacao_Jul2001_SP.tif", "Spot_Vegetacao_Jul2001_SP","geotiff", "astyle");
    geoserver.registerCoverageFile("acoverage", TERRAMA2_DATA_DIR + "/geotiff/L5219076_07620040908_r3g2b1.tif", "L5219076_07620040908_r3g2b1", "geotiff");

    // Removing the coverages
    geoserver.deleteCoverageLayer("acoverage", "Spot_Vegetacao_Jul2001_SP", true);
    geoserver.deleteCoverageLayer("acoverage", "L5219076_07620040908_r3g2b1", true);

    // Uploading many coverages from a zip file
    geoserver.uploadZipCoverageFile("acoverage", TERRAMA2_DATA_DIR + "/geotiff/geotiff.zip", "geotiff");

    //geoserver.registerMosaicCoverage("mosaicStore", TERRAMA2_DATA_DIR + "/grads", "Umin", "4326", "");

    std::list<std::pair<std::string, std::string>> layersAndStyles;

    layersAndStyles.push_back(std::make_pair("aworkspace:Spot_Vegetacao_Jul2001_SP", ""));
    layersAndStyles.push_back(std::make_pair("aworkspace:muni", "astyle"));
    layersAndStyles.push_back(std::make_pair("aworkspace:Rod_Principais_SP_lin", ""));

    te::gm::Envelope env(-53.11664642808698, -25.31237828099399,
                         -44.16072686935583, -19.772910107715056);

    geoserver.getMapWMS("/home/vinicius", "imagem.jpg", layersAndStyles, env, 768, 516, 4326, "image/jpeg");

    geoserver.deleteCoverageLayer("acoverage", "Spot_Vegetacao_Jul2001_SP", true);

    geoserver.deleteVectorLayer("ashapesfolder", "Rod_Principais_SP_lin", true);

    geoserver.deleteWorkspace(true);

  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in GeoServer example: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in GeoServer example!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
