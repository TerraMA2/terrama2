define([
  "TerraMA2WebApp/data-provider/controllers/index",
  "TerraMA2WebApp/data-provider/services",
  "TerraMA2WebApp/data-provider/directives"
], function(controllerModule, serviceModule, directives) {
  var moduleName = "terrama2.providers";

  angular.module(moduleName, [serviceModule, controllerModule, directives]);

  return moduleName;
});