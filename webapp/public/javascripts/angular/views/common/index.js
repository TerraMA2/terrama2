define([
  "TerraMA2WebApp/views/common/constants/style"
], function(StyleConstants) {
  var moduleName = "terrama2.views.common";

  angular.module(moduleName, [])
    .constant("StyleConstants", StyleConstants);

  return moduleName;
});