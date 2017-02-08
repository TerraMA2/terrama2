define([
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/geo/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/data-series/components/storager"
], function(dataSeriesApp, commonApp, geoApp, servicesApp, terrama2StoragerComponent){
    "use strict";

    var moduleName = "terrama2.dataseries.components";

    angular
        .module(moduleName, [dataSeriesApp, commonApp, geoApp, servicesApp])
        .component("terrama2StoragerForm", terrama2StoragerComponent);
    
    return moduleName;
});