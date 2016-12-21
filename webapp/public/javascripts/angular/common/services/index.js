define([
  "TerraMA2WebApp/common/services/dialog",
  "TerraMA2WebApp/common/services/save-as",
  "TerraMA2WebApp/common/services/socket",
  "TerraMA2WebApp/common/services/unique",
  "TerraMA2WebApp/common/services/request"
], function(FileDialog, SaveAs, Socket, UniqueNumber, BaseService) {
  var moduleName = "terrama2.common.services";
  angular.module(moduleName, [])
    .factory("FileDialog", FileDialog)
    .factory("SaveAs", SaveAs)
    .factory("Socket", Socket)
    .factory("UniqueNumber", UniqueNumber)
    .service("BaseService", BaseService);
  return moduleName;
});