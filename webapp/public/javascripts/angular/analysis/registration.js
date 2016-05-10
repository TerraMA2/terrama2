angular.module('terrama2.analysis.registration', ['terrama2', 'terrama2.services', 'terrama2.components.messagebox', 'schemaForm'])

  .controller('StoragerController', ['$scope', 'DataSeriesSemanticsFactory', function($scope, DataSeriesSemanticsFactory) {

    $scope.formStorager = [];
    $scope.modelStorager = {};
    $scope.schemaStorager = {};

    $scope.$on('storagerFormatChange', function(event, args) {
      $scope.formatSelected = args.format;
      // todo: fix it. It is hard code
      // $scope.tableFieldsStorager = ["table_name", "inputDataSet"];

      DataSeriesSemanticsFactory.get(args.format.name, {metadata:true}).success(function(data) {
        var metadata = data.metadata;
        var properties = metadata.schema.properties;

        $scope.modelStorager = {};
        $scope.formStorager = metadata.form;
        $scope.schemaStorager = {
          type: 'object',
          properties: metadata.schema.properties,
          required: metadata.schema.required
        };

        $scope.$broadcast('schemaFormRedraw');
      }).error(function(err) {

      });
    });
  }])

  .controller('AnalysisRegistration',
    [
      '$scope',
      'ServiceInstanceFactory',
      'DataSeriesFactory',
      'DataSeriesSemanticsFactory',
      'AnalysisFactory',
      'DataProviderFactory',
  function($scope, ServiceInstanceFactory, DataSeriesFactory, DataSeriesSemanticsFactory, AnalysisFactory, DataProviderFactory) {
    // initializing objects
    $scope.analysis = {};
    $scope.instances = [];
    $scope.dataSeriesList = [];
    $scope.selectedDataSeries = null;
    $scope.metadata = {};
    $scope.semantics = {};
    $scope.storagerFormats = [];

    // terrama2 alert box
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.close = function() {
      $scope.display = false;
    };

    DataSeriesSemanticsFactory.list().success(function(semanticsList) {
      $scope.dataSeriesSemantics = semanticsList;

      $scope.dataSeriesSemantics.forEach(function(dSemantics) {
        if(dSemantics.data_series_type_name === "ANALYSIS") {
          $scope.storagerFormats.push(Object.assign({}, dSemantics));
        }
      });
    }).error(function(err) {
      console.log(err);
    });

    DataProviderFactory.get().success(function(dataProviders) {
      $scope.dataProviders = dataProviders;
    }).error(function(err) {
      console.log(err);
    });

    $scope.onStoragerFormatChange = function() {
      $scope.showStoragerForm = true;
      $scope.$broadcast('storagerFormatChange', {format: $scope.storager.format});
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
      $scope.metadata.INFLUENCE_RADIUS_UNIT = format;
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
        if($scope.metadata.hasOwnProperty(key)) {
          metadata[key] = $scope.metadata[key];
        }
      }

      console.log($scope);

      var analysisDataSeries = {
        data_series_id: $scope.selectedDataSeries.id,
        metadata: {},
        alias: $scope.metadata.alias,
        // todo: check it
        type_id: $scope.analysis.type_id
      };

      var analysisToSend = Object.assign({}, $scope.analysis);
      analysisToSend.dataSeries = $scope.selectedDataSeries;
      analysisToSend.analysisDataSeries = analysisDataSeries;
      analysisToSend.metadata = metadata;

      var storager = Object.assign({}, $scope.storager, $scope.modelStorager);

      var storager = Object.assign({}, $scope.storager, $scope.modelStorager);


      // sending post operation
      AnalysisFactory.post({
        analysis: analysisToSend,
        storager: storager
      }).success(function(data) {
        alert("Saved");
        console.log(data);
      }).error(errorHelper);
    };
  }]);
