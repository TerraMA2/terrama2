var terrama2Application = angular.module("terrama2", []);

// It avoid conflicts with template engine tags. The angular tags will be {[ someAngularCommand ]}
terrama2Application.config(['$interpolateProvider', function($interpolateProvider) {
    $interpolateProvider.startSymbol('{[');
    $interpolateProvider.endSymbol(']}');
}]);

//terrama2Application.config(function($stateProvider, $urlRouterProvider) {
//
//  $stateProvider.state('main', {
//    abstract: true,
//    template: '<div ui-view=""></div>',
//    resolve: {
//      "i18nData": ["i18n", function (i18n) {
//        return i18n.ensureLocaleIsLoaded();
//      }]
//    }
//  });
//
//});