define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/data-series/controllers",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/components/index",
  "TerraMA2WebApp/data-series/directives"
], function(moduleLoader, controllersApp, servicesApp, scheduleApp, componentsApp, directives) {
  var moduleName = "terrama2.dataseries";

  var deps = [controllersApp, servicesApp, scheduleApp, componentsApp, directives];
  moduleLoader("ngFileUpload", deps);

  angular.module(moduleName, deps);

  return moduleName;
});
