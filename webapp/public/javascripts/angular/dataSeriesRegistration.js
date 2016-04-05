angular.module('terrama2.dataseries.registration', ['terrama2', 'ui.router', 'terrama2.projection', 'terrama2.services'])
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

    $urlRouterProvider.otherwise(function ($injector, $location) {
      var $state = $injector.get('$state');

      if ($location.$$hash.indexOf("/wizard" == -1) || $location.$$hash.indexOf("/advanced") == -1) 
        if ($state.current.name)
          $state.go($state.current.name);
        else
          $state.go('advanced');
    });

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

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsFactory", "DataProviderFactory", "DataSeriesFactory",
    function($scope, $http, i18n, $window, $state, $httpParamSerializer, DataSeriesSemanticsFactory, DataProviderFactory, DataSeriesFactory) {

      $scope.stateApp = $state;
      $scope.semantics = "";
      $scope.isDynamic = params.state.toLowerCase() === "dynamic";
      $scope.pcds = [];
      $scope.parametersData = configuration.parametersData || {};

      $scope.dataSeries = {
        data_provider_id: configuration.dataSeries.dataProvider || "",
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
              $scope.onDataSemanticsChange();
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
        var output = Object.assign({}, $scope.dataSeries);
        output.semantics = semanticsName;
        output.parametersData = $scope.parametersData;

        $window.location.href = "/configuration/providers/new?redirectTo=" + url + "&" + $httpParamSerializer(output);
      };

      $scope.removePcd = function(pcdItem) {
        $scope.pcds.forEach(function(pcd, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (pcd.mask === pcdItem.mask) {
            array.splice(pcdIndex, 1);
            return;
          }
        });
      };

      $scope.addHelperMask = function(maskFormat) {
        if ($scope.parametersData && $scope.parametersData.mask)
          $scope.parametersData.mask += maskFormat;
        else
          $scope.parametersData.mask = maskFormat;
      };

      $scope.addPcd = function(pcd) {
        if (this.parametersDataForm.$valid) {

          $scope.pcds.push(pcd);
          var path = $scope.parametersData.path;

          angular.element("body").focus();

          this.parametersDataForm.$setPristine();
          this.parametersDataForm.$setUntouched();

          $scope.parametersData = {path: path};
        }
      };

      $scope.changeDataProvider = function() {
        console.log($scope.dataSeries);
      };

      $scope.save = function() {
        var dataToSend = Object.assign({}, $scope.dataSeries);
        dataToSend.data_series_semantic_name = $scope.dataSeries.semantics.name;

        var semantics = dataToSend.semantics;
        delete dataToSend.semantics;

        dataToSend.dataSets = [];

        switch(semantics.data_series_type_name.toLowerCase()) {
          case "dcp":
          case "pcd":
            $scope.pcds.forEach(function(pcd) {
              var dataSetStructure = {
                semantics: semantics,
                active: pcd.active,
                child: {
                  position: {
                    type: 'Point',
                    coordinates: [pcd.latitude, pcd.longitude],
                    crs:{
                      type: 'name',
                      properties : {
                        name: pcd.projection
                      }
                    }
                  }
                }
              };

              dataToSend.dataSets.push(dataSetStructure);
            });

            break;

          case "occurrence":
            dataToSend.dataSets.push({type: dataSetType, active: this.parametersDataForm.active});
            break;

          case "grid":
            break;

          default:
            break;
        }

        console.log(dataToSend);
        DataSeriesFactory.post(dataToSend).success(function(data) {
          //   redirect to list data series || analysis: TODO
          console.log(data);
        }).error(function(err) {
          console.log(err);
          alert("Error found")
        });

      };
    }
  ]);