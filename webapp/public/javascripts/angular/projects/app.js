define([
  "TerraMA2WebApp/projects/controllers/index"
], function(controllersModule) {
  var moduleName = "terrama2.projects";

  angular.module(moduleName, [controllersModule]);

  return moduleName;
});