define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/views/services/view",
  "TerraMA2WebApp/views/services/color"
], function(commonService, ViewService, ColorFactory) {
  var moduleName = "terrama2.views.services";

  angular.module(moduleName, [commonService])
    .service("ViewService", ViewService)
    .factory("ColorFactory", ColorFactory);
  
  return moduleName;
});