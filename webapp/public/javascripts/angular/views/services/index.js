define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/views/services/view"
], function(commonService, ViewService) {
  var moduleName = "terrama2.views.services";

  angular.module(moduleName, [commonService])
    .service("ViewService", ViewService);
  
  return moduleName;
});