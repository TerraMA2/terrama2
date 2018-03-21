define([
  "TerraMA2WebApp/interpolator/services/interpolator"
], function(InterpolatorService) {
  var moduleName = "terrama2.interpolator.services";

  angular.module(moduleName, [])
    .service("InterpolatorService", InterpolatorService);
  
  return moduleName;
});