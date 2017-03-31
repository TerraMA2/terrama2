define([
  "TerraMA2WebApp/views/controllers/index"
], function(controllersModule){
  'use strict';

  var moduleName = "terrama2.alerts";

  angular.module(moduleName, [controllersModule]);

  return moduleName;
});