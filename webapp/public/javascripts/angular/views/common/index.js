define([
  "TerraMA2WebApp/views/common/constants/style-type"
], function(StyleType) {
  var moduleName = "terrama2.views.common";

  angular.module(moduleName, [])
    .constant("StyleType", StyleType);

  return moduleName;
});