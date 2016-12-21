define([
  "TerraMA2WebApp/projects/controllers/index"
], function(controllersModule) {
  var moduleName = "terrama2.projects";

  angular.module(moduleName, [controllersModule]);

  return moduleName;
})

angular.module('terrama2.projects', ['terrama2', 'terrama2.table', 'terrama2.components.messagebox', 'terrama2.services']);