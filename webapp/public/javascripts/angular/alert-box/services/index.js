define([
  "TerraMA2WebApp/alert-box/common/index",
  "TerraMA2WebApp/alert-box/services/messagebox"
], function(commonModule, MessageBoxService) {
  var moduleName = "terrama2.components.messagebox.services";

  angular.module(moduleName, [commonModule])
    .service("MessageBoxService", MessageBoxService);

  return moduleName;
})