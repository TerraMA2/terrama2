define([
  // "TerraMA2WebApp/data-series/controllers",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule"
], function(servicesApp, scheduleApp) {
  var moduleName = "terrama2.dataseries";

  angular.module(moduleName, [servicesApp, scheduleApp]);

  return moduleName;
});