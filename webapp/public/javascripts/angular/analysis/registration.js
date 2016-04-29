angular.module('terrama2.analysis.registration', ['terrama2', 'terrama2.services'])
  .controller('AnalysisRegistration', ['$scope', 'ServiceInstanceFactory', 'DataSeriesFactory', function($scope, ServiceInstanceFactory, DataSeriesFactory) {
    // initializing objects
    $scope.analysis = {};
    $scope.instances = [];
    $scope.dataSeriesList = [];
    $scope.selectedDataSeries = null;
    $scope.metadata = {};
    
    // temp code for debugging
    var errorHelper = function(err) {
      console.log(err);
    };
    
    // getting instances
    ServiceInstanceFactory.get({type: 'ANALYSIS'}).success(function(services) {
      $scope.instances = services;
    }).error(errorHelper);
    
    // getting DataSeries
    DataSeriesFactory.get().success(function(dataSeriesObjects) {
      $scope.dataSeriesList = dataSeriesObjects;
    }).error(errorHelper);
    
    // helpers
    var formErrorDisplay = function(form) {
      angular.forEach(form.$error, function (field) {
        angular.forEach(field, function(errorField){
          errorField.$setDirty();
        })
      });
    };

    // handling functions
    // it handles hidden box with data-series analysis metadata
    $scope.onDataSeriesClick = function(dataSeries) {
      $scope.selectedDataSeries = dataSeries;
    };

    $scope.onMetadataFormatClick = function(format) {
      $scope.metadata.format = format;
    };
    
    // save function
    $scope.save = function() {
      console.log($scope.analysis);
      if ($scope.generalDataForm.$invalid) {
        formErrorDisplay($scope.generalDataForm);
        return;
      }
      
      
    };
  }]);