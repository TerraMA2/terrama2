define([
  "TerraMA2WebApp/interpolator/controllers/interpolator-register-update"
], function(RegisterUpdateController){
  var moduleName = "terrama2.interpolator.controllers";

  var deps = [];

  angular.module(moduleName, deps)
    .controller("InterpolatorRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});