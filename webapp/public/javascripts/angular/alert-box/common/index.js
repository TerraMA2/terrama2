define([
    "TerraMA2WebApp/alert-box/common/constants/alert-level"
  ], 
  function(AlertLevel) {
    var moduleName = "terrama2.components.messagebox.common";

    angular.module(moduleName, [])
      .constant("AlertLevel", AlertLevel)

    return moduleName; 
  }
)