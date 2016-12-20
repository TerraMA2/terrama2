define([
  "TerraMA2WebApp/common/services/dialog",
  "TerraMA2WebApp/common/services/save-as",
  "TerraMA2WebApp/common/services/socket",
  "TerraMA2WebApp/common/services/unique"
], function(FileDialog, SaveAs, Socket, UniqueNumber) {
  var moduleName = "terrama2.common.services";
  angular.module(moduleName, [])
    .factory("FileDialog", FileDialog)
    .factory("SaveAs", SaveAs)
    .factory("Socket", Socket)
    .factory("UniqueNumber", UniqueNumber);
  return moduleName;
});