define([
  "TerraMA2WebApp/common/services/dialog",
  "TerraMA2WebApp/common/services/save-as",
  "TerraMA2WebApp/common/services/socket",
  "TerraMA2WebApp/common/services/unique",
  "TerraMA2WebApp/common/services/request",
  "TerraMA2WebApp/common/services/http-timeout",
  "TerraMA2WebApp/common/services/string-format",
  "TerraMA2WebApp/common/services/date-parser",
  "TerraMA2WebApp/common/services/uri-parser",
  "TerraMA2WebApp/common/services/form-translator",
  "TerraMA2WebApp/common/services/utility",
  "./debounce"
], function(FileDialog, SaveAs, Socket, UniqueNumber, BaseService, HttpTimeout, StringFormat, DateParser, URIParser, FormTranslator, Utility, debounce) {
  var moduleName = "terrama2.common.services";
  angular.module(moduleName, [])
    .factory("FileDialog", FileDialog)
    .factory("Utility", Utility)
    .factory("SaveAs", SaveAs)
    .factory("Socket", Socket)
    .factory("UniqueNumber", UniqueNumber)
    .factory("$HttpTimeout", HttpTimeout)
    .factory("StringFormat", StringFormat)
    .factory("DateParser", DateParser)
    .factory("URIParser", URIParser)
    .factory('debounce', debounce)
    .factory("FormTranslator", FormTranslator)
    .service("BaseService", BaseService);
  return moduleName;
});
