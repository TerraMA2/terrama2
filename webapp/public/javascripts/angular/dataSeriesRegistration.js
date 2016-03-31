angular.module('terrama2.dataseries.registration', ['terrama2', 'ui.router', 'terrama2.projection', 'terrama2.services'])
  .config(["$locationProvider", function($locationProvider) {
    $locationProvider.hashPrefix('!');
  }])

  .config(["$stateProvider", "$urlRouterProvider", function($stateProvider, $urlRouterProvider) {
    $stateProvider.state('main', {
      abstract: true,
      template: '<div ui-view=""></div>',
      resolve: {
        "i18nData": ["i18n", function (i18n) {
          return i18n.ensureLocaleIsLoaded();
        }]
      }
    });

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
  }])

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsFactory", "DataProviderFactory", 
    function($scope, $http, i18n, $window, $state, $httpParamSerializer, DataSeriesSemanticsFactory, DataProviderFactory) {

      $scope.stateApp = $state;
      $scope.semantics = "";
      $scope.isDynamic = params.state.toLowerCase() === "dynamic";

      $scope.dataSeries = {
        dataProvider: configuration.dataSeries.dataProvider || "",
        name: configuration.dataSeries.name || "",
        access: configuration.dataSeries.access
      };

      $scope.dataSeries.semantics = {};

      DataSeriesSemanticsFactory.get().success(function(semanticsList) {
        $scope.dataSeriesSemantics = semanticsList;

        if (configuration.dataSeries.semantics) {
          semanticsList.forEach(function(semantics) {
            if (semantics.name === configuration.dataSeries.semantics) {
              $scope.dataSeries.semantics = semantics;
              return;
            }
          })
        }

      }).error(function(err) {
        console.log(err);
      });

      DataProviderFactory.get().success(function(dataProviders) {
        $scope.dataProviders = dataProviders;
      }).error(function(err) {
        console.log(err);
      });

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

      // it defines when data change combobox has changed and it will adapt the interface
      $scope.onDataSemanticsChange = function() {
        $scope.semantics = $scope.dataSeries.semantics.data_format_name.toLowerCase();
        
      };

      $scope.onDataProviderClick = function(index) {

        var url = $window.location.pathname + "&type=" + params.state;

        var semanticsName = $scope.dataSeries.semantics.name || "";

        $window.location.href = "/configuration/providers/new?redirectTo=" + url + "&" + $httpParamSerializer(Object.assign({semantics: semanticsName}, $scope.dataSeries));
        // $window.open("/configuration/providers/new", "Data Provider Registration", 'width=600,height=500,scrollbars=yes');
      };

      $scope.changeDataProvider = function() {
        console.log($scope.dataSeries);
      };
    }
  ]);