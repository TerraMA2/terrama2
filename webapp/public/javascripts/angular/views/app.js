define([
  "TerraMA2WebApp/views/common/index",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/views/components/index",
  "TerraMA2WebApp/views/controllers/index"
], function(commonViewModule, serviceModule, componentsApp, controllersModule) {
  'use strict';

  var moduleName = "terrama2.views";

  angular.module(moduleName, [commonViewModule, serviceModule, componentsApp, controllersModule]);

  return moduleName;
});