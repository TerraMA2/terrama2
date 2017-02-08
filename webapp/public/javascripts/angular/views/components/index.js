define([
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/views/common/index",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/views/components/style"
], function(dataSeriesApp, coreApp, servicesApp, terrama2StyleComponent) {
  "use strict";

  var moduleName = "terrama2.views.components";

  angular
    .module(moduleName, [coreApp, dataSeriesApp, servicesApp])
    .component("terrama2StyleLegend", terrama2StyleComponent);

  return moduleName;
});