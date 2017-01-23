define([
  "TerraMA2WebApp/views/common/constants/style-type",
  "TerraMA2WebApp/views/common/constants/style-operation"
], function(StyleType, StyleOperation) {
  var moduleName = "terrama2.views.common";

  angular.module(moduleName, [])
    .constant("StyleType", StyleType)
    .constant("StyleOperation", StyleOperation);

  return moduleName;
});