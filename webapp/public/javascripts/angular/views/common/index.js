define([
  "TerraMA2WebApp/views/common/constants/style",
  "TerraMA2WebApp/views/common/constants/style-type"
], function(StyleConstants, StyleType) {
  var moduleName = "terrama2.views.common";

  angular.module(moduleName, [])
    .constant("StyleType", StyleType)
    .constant("StyleConstants", StyleConstants);

  return moduleName;
});