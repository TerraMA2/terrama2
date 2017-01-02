define([
  "TerraMA2WebApp/data-provider/controllers/index",
  "TerraMA2WebApp/data-provider/services"
], function(controllerModule, serviceModule) {
  var moduleName = "terrama2.providers";

  angular.module(moduleName, [serviceModule, controllerModule]);

  return moduleName;
});