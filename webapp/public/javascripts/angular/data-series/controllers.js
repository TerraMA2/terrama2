define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/datetimepicker/directive",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/data-provider/services",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/directives",
  "TerraMA2WebApp/geo/app",

  // controllers
  "TerraMA2WebApp/data-series/data-series",
  "TerraMA2WebApp/data-series/registration",
  "TerraMA2WebApp/schema-form-plugin/mask-warn/directives/terrama2-mask-field"
], function(moduleLoader, commonServiceApp, messageboxApp, datetimepickerApp, providerApp, serviceApp, 
            dataSeriesServicesApp, scheduleApp, directives, geoApp, ListController, RegistrationController) {
  var moduleName = "terrama2.dataseries.controllers";
  var deps = [commonServiceApp, messageboxApp, directives];

  // checking externals dependencies
  if (moduleLoader("schemaForm", deps) && 
      moduleLoader("xeditable", deps) && 
      moduleLoader("treeControl", deps) &&
      moduleLoader("ui.router", deps) && 
      moduleLoader("ui.select", deps) &&
      moduleLoader("ngSanitize", deps) &&
      moduleLoader("mgo-angular-wizard", deps)) {
    deps.push(commonServiceApp);
    deps.push(providerApp);
    deps.push(serviceApp);
    deps.push(dataSeriesServicesApp);
    deps.push(scheduleApp);
    deps.push(datetimepickerApp);
    deps.push(geoApp);
  }

  var app = angular.module(moduleName, deps);

  app.controller("DataSeriesListController", ListController);
  
  if (deps.indexOf("ui.router") !== -1) {

    app
      .controller("DataSeriesRegisterUpdateController", RegistrationController.RegisterDataSeries)
      .config(["$stateProvider", "$urlRouterProvider", function($stateProvider, $urlRouterProvider) {
        $stateProvider.state('main', {
          abstract: true,
          template: '<div ui-view=""></div>',
          resolve: {
            "i18nData": ["i18n", function (i18n) {
              return i18n.ensureLocaleIsLoaded();
            }]
          },
          controller: 'DataSeriesRegisterUpdateController'
        });

        $urlRouterProvider.otherwise('wizard');

        $stateProvider
          .state('wizard', {
            parent: 'main',
            url: "/wizard",
            templateUrl: '/javascripts/angular/wizard.html'
          }).state('advanced', {
            parent: 'main',
            url: "/advanced",
            templateUrl: '/javascripts/angular/advanced.html'
          }
        );
      }])
      .run(["editableOptions", function(editableOptions) {
        editableOptions.theme = 'bs3'; // bootstrap3 theme. Can be also 'bs2', 'default'
      }]);
  }

  return moduleName;
});
