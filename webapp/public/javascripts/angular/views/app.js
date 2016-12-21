define([
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/views/controllers/index"
], function(serviceModule, controllersModule) {
  'use strict';

  var moduleName = "terrama2.views";

  angular.module(moduleName, [serviceModule, controllersModule]);

  return moduleName;
}());