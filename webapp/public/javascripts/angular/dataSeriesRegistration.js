angular.module('terrama2.dataseries.registration', ['terrama2', 'terrama2.projection'])

  .config(function($stateProvider, $urlRouterProvider) {
    $urlRouterProvider.otherwise("/wizard");

    $stateProvider
      .state('wizardMode', {
        parent: 'main',
        url: "/wizard",
        templateUrl: '/javascripts/angular/wizard.html'
      }).state('advancedMode', {
        parent: 'main',
        url: "/advanced",
        templateUrl: '/javascripts/angular/advanced.html'
      }
    );
  })

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', function($scope, $http, i18n) {
    $scope.i18n = i18n;
    $scope.isWizard = true;
    $scope.projection = "";

    // change form: advanced or wizard
    $scope.onFormView = function() {
      $scope.isWizard = !$scope.isWizard;
    };

    // parameters to handle fields display
    $scope.params = params;
    $scope.dataSets = [];

    $scope.dataSet = {};

    // it defines when data change combobox has changed and it will adapt the interface
    $scope.onChangeDataSetKind = function() {

    };
  }]);
