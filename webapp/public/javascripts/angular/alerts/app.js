define([
  "TerraMA2WebApp/alerts/controllers/index",
  "TerraMA2WebApp/alerts/services/index",
], function(controllersModule, serviceModule){
  'use strict';

  var moduleName = "terrama2.alerts";

  angular.module(moduleName, [controllersModule, serviceModule]);

  return moduleName;
});