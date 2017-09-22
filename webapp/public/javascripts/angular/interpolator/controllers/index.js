define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/interpolator/controllers/interpolator-register-update"
], function(moduleLoader, messageBoxApp, serviceApp, RegisterUpdateController){
  var moduleName = "terrama2.interpolator.controllers";

  var deps = [messageBoxApp];
  moduleLoader(serviceApp, deps);

  angular.module(moduleName, deps)
    .controller("InterpolatorRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});