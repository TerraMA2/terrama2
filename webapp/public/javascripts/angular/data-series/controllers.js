define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/services/index"
  "TerraMA2WebApp/alert-box/app",

  // controllers
  "TerraMA2WebApp/data-series/data-series",
  "TerraMA2WebApp/data-series/registration"
], function(moduleLoader, commonServiceApp, messageboxApp, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.dataseries.controllers";
  var deps = [commonServiceApp];

    //   'terrama2',
    // 'terrama2.services',
    // 'terrama2.dataseries.services',
    // 'terrama2.components.messagebox', // handling alert box
    // 'terrama2.components.messagebox.services',
    // 'ui.router',
    // 'mgo-angular-wizard', // wizard
    // 'schemaForm',
    // 'xeditable',
    // 'terrama2.schedule',
    // 'terrama2.datetimepicker',
    // 'terrama2.components.geo',
    // 'treeControl',

  angular.module(moduleName, deps)
    .controller("DataSeriesListController", ListController)
    .controller("DataSeriesRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});