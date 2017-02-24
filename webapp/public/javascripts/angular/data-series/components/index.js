define([
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/geo/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/data-series/components/storager",
  "TerraMA2WebApp/data-series/components/csvFormat",
  "TerraMA2WebApp/data-series/components/csvFormatDetail"
], function(dataSeriesApp, commonApp, geoApp, servicesApp, terrama2StoragerComponent, csvFormat, csvFormatDetail){
    "use strict";

    var moduleName = "terrama2.dataseries.components";

    angular
        .module(moduleName, [dataSeriesApp, commonApp, geoApp, servicesApp])
        .component("terrama2StoragerForm", terrama2StoragerComponent)
        .component("csvFormat", csvFormat)
        .component("csvFormatDetail", csvFormatDetail);
    
    return moduleName;
});