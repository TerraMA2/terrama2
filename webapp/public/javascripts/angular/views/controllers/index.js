define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/views/common/index",

  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/datetimepicker/directive",
  "TerraMA2WebApp/ace/directive",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",

  "TerraMA2WebApp/views/controllers/view-list",
  "TerraMA2WebApp/views/controllers/view-register-update"
], function(moduleLoader, commonApp, commonViewApp, scheduleApp, dataSeriesServiceApp, viewsService, datetimepicker, 
            aceApp, messageBoxApp, serviceApp, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.views.controllers";

  var deps = [commonViewApp, messageBoxApp, viewsService];

  // controller RegisterUpdate dependencies
  if (moduleLoader("schemaForm", deps) && moduleLoader("color.picker", deps)) {
    moduleLoader("xeditable", deps);
    moduleLoader(dataSeriesServiceApp, deps);
    moduleLoader(aceApp, deps);
    moduleLoader(datetimepicker, deps);
    moduleLoader(serviceApp, deps);
    moduleLoader(scheduleApp, deps);
  }

  angular.module(moduleName, deps)
    .controller("ViewListController", ListController)
    .controller("ViewRegisterUpdateController", RegisterUpdateController);
  
  if (deps.indexOf("xeditable") !== -1) {
    angular.module(moduleName)
      .run(["editableOptions", function(editableOptions) {
        editableOptions.theme = 'bs3';
      }]);
  }

  return moduleName;
})