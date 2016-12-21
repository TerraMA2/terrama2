define([
  "TerraMA2WebApp/views/services/view"
], function(ViewService) {
  var moduleName = "terrama2.views.services";

  angular.module(moduleName, [])
    .service("ViewService", ViewService);
  
  return moduleName
});