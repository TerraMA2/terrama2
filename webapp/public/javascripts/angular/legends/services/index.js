define([
  "TerraMA2WebApp/legends/services/legend"
], function(LegendService) {
  var moduleName = "terrama2.legends.services";

  angular.module(moduleName, []).service("LegendService", LegendService);

  return moduleName;
});