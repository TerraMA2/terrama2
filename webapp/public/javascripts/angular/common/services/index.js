define([
  "TerraMA2WebApp/common/services/dialog",
  "TerraMA2WebApp/common/services/save-as",
  "TerraMA2WebApp/common/services/socket",
  "TerraMA2WebApp/common/services/unique",
  "TerraMA2WebApp/common/services/request",
  "TerraMA2WebApp/common/services/http-timeout",
  "TerraMA2WebApp/common/services/data-provider",
  "TerraMA2WebApp/common/services/string-format"
], function(FileDialog, SaveAs, Socket, UniqueNumber, BaseService, HttpTimeout, DataProviderService, StringFormat) {
  var moduleName = "terrama2.common.services";
  angular.module(moduleName, [])
    .factory("FileDialog", FileDialog)
    .factory("SaveAs", SaveAs)
    .factory("Socket", Socket)
    .factory("UniqueNumber", UniqueNumber)
    .factory("$HttpTimeout", HttpTimeout)
    .factory("StringFormat", StringFormat)
    .service("BaseService", BaseService)
    .service("DataProviderService", DataProviderService);
  return moduleName;
});