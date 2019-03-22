define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/data-series/controllers",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/components/index",
  "TerraMA2WebApp/data-series/directives",
  // "TerraMA2WebApp/data-series/components/attributes"
], function(moduleLoader, controllersApp, servicesApp, scheduleApp, componentsApp, directives/*, attributes*/) {
  var moduleName = "terrama2.dataseries";

  var deps = [controllersApp, servicesApp, scheduleApp, componentsApp, directives];
  moduleLoader("ngFileUpload", deps);

  angular.module(moduleName, deps)
  .component("attributes", attributes)

  return moduleName;
});