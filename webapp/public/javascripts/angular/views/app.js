define([
  "TerraMA2WebApp/views/common/index",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/data-provider/app",
  "TerraMA2WebApp/views/components/index",
  "TerraMA2WebApp/views/controllers/index",
  "TerraMA2WebApp/views/components/chart",
  "TerraMA2WebApp/common/loader",
], function(commonViewModule, serviceModule, providerApp, componentsApp, controllersModule, chart, loader) {
  'use strict';

  var moduleName = "terrama2.views";

  var deps = [commonViewModule, serviceModule, providerApp, componentsApp, controllersModule];
  loader("schemaForm", deps);
  angular.module(
    moduleName,
    deps
  )
  .component("chart", chart)
  ;

  return moduleName;
});