define([
  "TerraMA2WebApp/legends/controllers/index",
  "TerraMA2WebApp/legends/services/index",
], function(controllersModule, serviceModule) {
  'use strict';

  var moduleName = "terrama2.legends";

  angular.module(moduleName, [controllersModule, serviceModule]);

  return moduleName;
});