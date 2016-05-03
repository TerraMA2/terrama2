angular.module('terrama2.analysis.registration', ['terrama2', 'terrama2.services', 'terrama2.components.messagebox'])
  .controller('AnalysisRegistration',
    [
      '$scope',
      'ServiceInstanceFactory',
      'DataSeriesFactory',
      'DataSeriesSemanticsFactory',
      'AnalysisFactory',
  function($scope, ServiceInstanceFactory, DataSeriesFactory, DataSeriesSemanticsFactory, AnalysisFactory) {
    // initializing objects
    $scope.analysis = {};
    $scope.instances = [];
    $scope.dataSeriesList = [];
    $scope.selectedDataSeries = null;
    $scope.metadata = {};
    $scope.semantics = {};

    // terrama2 alert box
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.close = function() {
      $scope.display = false;
    };

    
    // temp code for debugging
    var errorHelper = function(err) {
      console.log(err);
    };
    
    // getting instances
    ServiceInstanceFactory.get({type: 'ANALYSIS'}).success(function(services) {
      $scope.instances = services;
    }).error(errorHelper);
    
    // getting DataSeries
    DataSeriesFactory.get({collector: true}).success(function(dataSeriesObjects) {
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
      
      // getting data series semantics
      DataSeriesSemanticsFactory.get(dataSeries.semantics).success(function(data) {
        $scope.semantics = data;
      }).error(errorHelper);
    };

    // pcd metadata (radius format - km, m...)
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

      if ($scope.scriptForm.$invalid) {
        formErrorDisplay($scope.scriptForm);
        return;
      }

      // todo: improve it
      // temp code for sending analysis dataseries
      var metadata = {};
      for(var key in $scope.metadata) {
        if ($scope.metadata.hasOwnProperty(key)) {
          metadata[key] = $scope.metadata[key];
        }
      }

      var analysisDataSeries = {
        data_series_id: $scope.selectedDataSeries.id,
        metadata: metadata
      };

      var analysisToSend = Object.assign({}, $scope.analysis);
      analysisToSend.analysisDataSeries = analysisDataSeries;
      
      // sending post operation
      AnalysisFactory.post(analysisToSend).success(function(data) {
        alert("Saved");
        console.log(data);
      }).error(errorHelper);
    };
  }]);