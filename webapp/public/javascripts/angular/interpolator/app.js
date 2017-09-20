define([
  "TerraMA2WebApp/interpolator/controllers/index",
  "TerraMA2WebApp/interpolator/services/index"
], function(controllersModule, serviceModule){
  'use strict';

  var moduleName = "terrama2.interpolator";

  angular.module(moduleName, [controllersModule, serviceModule]);

  return moduleName;
});