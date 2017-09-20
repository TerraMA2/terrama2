define([
  "TerraMA2WebApp/interpolator/controllers/index"
], function(controllersModule){
  'use strict';

  var moduleName = "terrama2.interpolator";

  angular.module(moduleName, [controllersModule]);

  return moduleName;
});