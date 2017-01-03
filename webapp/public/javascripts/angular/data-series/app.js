define([
  "TerraMA2WebApp/data-series/controllers",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule"
], function(controllersApp, servicesApp, scheduleApp) {
  var moduleName = "terrama2.dataseries";

  angular.module(moduleName, [controllersApp, servicesApp, scheduleApp]);

  return moduleName;
});