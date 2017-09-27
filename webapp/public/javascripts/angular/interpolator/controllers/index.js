define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/interpolator/controllers/interpolator-register-update"
], function(moduleLoader, messageBoxApp, serviceApp, RegisterUpdateController){
  var moduleName = "terrama2.interpolator.controllers";

  var deps = [messageBoxApp];
  if (moduleLoader("ui.router", deps) &&
      moduleLoader("mgo-angular-wizard", deps) &&
      moduleLoader("ui.select", deps)){
    deps.push(serviceApp);

  }
  var app = angular.module(moduleName, deps);
  
  if (deps.indexOf("ui.router") !== -1) {
    app
      .controller("InterpolatorRegisterUpdateController", RegisterUpdateController)
      .config(["$stateProvider", "$urlRouterProvider", function($stateProvider, $urlRouterProvider){
        $stateProvider.state('inter', {
          abstract: true,
          template: '<div ui-view=""></div>',
          resolve: {
            "i18nData": ["i18n", function (i18n) {
              return i18n.ensureLocaleIsLoaded();
            }]
          },
          controller: 'InterpolatorRegisterUpdateController'
        });

        $urlRouterProvider.otherwise('wizard');

        $stateProvider
          .state('inter-wizard', {
            parent: 'inter',
            url: "/wizard",
            templateUrl: BASE_URL + 'javascripts/angular/interpolator/templates/wizard.html'
          }).state('inter-advanced', {
            parent: 'inter',
            url: "/advanced",
            templateUrl: BASE_URL + 'javascripts/angular/interpolator/templates/advanced.html'
          }
        );
      }])

  }

  return moduleName;
});