define([
  "TerraMA2WebApp/data-provider/controllers/index"
], function(controllerModule) {
  var moduleName = "terrama2.providers";

  angular.module(moduleName, [controllerModule]);

  return moduleName;
});