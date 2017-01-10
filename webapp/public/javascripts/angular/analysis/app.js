define([
  "TerraMA2WebApp/analysis/controllers/index",
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/analysis/directives"
], function(controllersModule, servicesModule, directivesModule) {
  var moduleName = "terrama2.analysis";

  angular.module(moduleName, [servicesModule, directivesModule, controllersModule]);

  return moduleName;
})