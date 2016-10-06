(function() {
  'use strict';

  angular.module("terrama2.views.controllers", [
      "terrama2",
      "terrama2.views.services",
      "schemaForm",
      "terrama2.dataseries.services",
      "terrama2.table",
      "terrama2.ace",
      "terrama2.components.messagebox.services",
      "terrama2.components.messagebox"])
    .controller("ViewRegisterUpdate", ViewRegisterUpdate)
    .controller("ViewList", ViewList);
}());