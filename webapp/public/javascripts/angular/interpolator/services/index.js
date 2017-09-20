define([
  "TerraMA2WebApp/interpolation/services/interpolation"
], function(InterpolationService) {
  var moduleName = "terrama2.interpolation.services";

  angular.module(moduleName, [])
    .service("InterpolationService", InterpolationService);
  
  return moduleName;
});