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

    $urlRouterProvider.otherwise("/advanced");

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

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsFactory", "DataProviderFactory", "DataSeriesFactory",
    function($scope, $http, i18n, $window, $state, $httpParamSerializer, DataSeriesSemanticsFactory, DataProviderFactory, DataSeriesFactory) {

      $scope.stateApp = $state;
      $scope.semantics = "";
      $scope.isDynamic = params.state.toLowerCase() === "dynamic";
      $scope.pcds = [];
      $scope.parametersData = {};


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

          angular.element("terrama2-add-pcd").focus();

          this.parametersDataForm.$setUntouched();

          $scope.parametersData = {path: path};
        }
      };

      $scope.changeDataProvider = function() {
        console.log($scope.dataSeries);
      };

      $scope.save = function() {
        console.log("GeneralData Form: ", $scope.generalDataForm);
        console.log("ParametersData Form: ", $scope.parametersDataForm);
        console.log("Filter Form: ", $scope.filterForm);

        var dataToSend = Object.assign({}, $scope.dataSeries);
        dataToSend.data_series_semantics_name = $scope.dataSeries.semantics.name;

        var dataSetType = dataToSend.semantics.data_series_type_name;
        delete dataToSend.semantics;

        dataToSend.dataSets = [];

        switch(dataSetType.toLowerCase()) {
          case "dcp":
          case "pcd":
            $scope.pcds.forEach(function(pcd) {
              var dataSetStructure = {
                type: dataSetType,
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

        // preparing data set array to send
        // $scope.dataSets.forEach(function(dataSet) {
        //   var dataSetStructure = {
        //     active: dataSet.active,
        //     child: {}
        //   };
        //
        //   dataSetStructure.child.type = dataSetType;
        //   switch(dataSetType.toLowerCase()) {
        //     case "dcp":
        //     case "pcd":
        //       $scope.pcds.forEach(function(pcd) {
        //
        //       });
        //
        //       dataSetStructure.child.position = {
        //         type: 'Point',
        //         coordinates: [$scope.parametersData.latitude, $scope.parametersData.longitude],
        //         crs:{
        //           type: 'name',
        //           properties : {
        //             name: 'EPSG:4326'
        //           }
        //         }
        //       };
        //
        //       break;
        //
        //     case "occurrence":
        //       break;
        //
        //     case "grid":
        //       break;
        //
        //     default:
        //       break;
        //   }
        // });

        console.log(dataToSend);
        DataSeriesFactory.post(dataToSend).success(function(data) {
          //   redirect to list data series || analysis
          console.log(data);
          alert("Foi");
        }).error(function(err) {
          console.log(err);
          alert("Error found")
        });

      };
    }
  ]);