define([
  "TerraMA2WebApp/alerts/controllers/index",
  "TerraMA2WebApp/alerts/services/index",
  "TerraMA2WebApp/legends/services/index",
], function(controllersModule, serviceModule, legendServiceModule){
  'use strict';

  var moduleName = "terrama2.alerts";

  angular.module(moduleName, [controllersModule, serviceModule, legendServiceModule]);

  return moduleName;
});