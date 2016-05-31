angular.module('terrama2.analysis.registration', [
    'terrama2',
    'terrama2.services',
    'terrama2.components.messagebox',
    'schemaForm',
    'treeControl'
  ])

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

    // define dataseries selected in modal
    $scope.selectedsDataSeries = [];

    // define dataseries selected to analysis
    $scope.selectedDataSeriesList = [];
    $scope.metadata = {};
    $scope.semantics = {};
    $scope.storagerFormats = [];
    $scope.buffers = {
      "static": [],
      "dynamic": []
    }

    // dataseries tree modal
    $scope.treeOptions = {
      nodeChildren: "children",
      multiSelection: true,
      dirSelectable: false,
      injectClasses: {
        ul: "list-group",
        li: "list-group-item",
        liSelected: "active",
        iExpanded: "a3",
        iCollapsed: "a4",
        iLeaf: "a5",
        label: "a6",
        labelSelected: "2"
      }
    }

    $scope.dataSeriesGroups = [
      {name: "Static", children: []},
      {name: "Dynamic", children: []}
    ]

    // terrama2 alert box
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.close = function() {
      $scope.display = false;
    };

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
    DataSeriesFactory.get({schema: "all"}).success(function(dataSeriesObjects) {
      $scope.dataSeriesList = dataSeriesObjects;

      dataSeriesObjects.forEach(function(dSeries) {
        var semantics = dSeries.data_series_semantics;

        if (semantics.data_series_type_name == "STATIC_DATA") {
          dSeries.isDynamic = false;
          $scope.buffers["static"].push(dSeries);
        }
        else {
          dSeries.isDynamic = true;
          $scope.buffers["dynamic"].push(dSeries);
        }
      });

      $scope.dataSeriesGroups[0].children = $scope.buffers["static"];
      $scope.dataSeriesGroups[1].children = $scope.buffers["dynamic"];
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
    // it adds dataseries from modal to table
    $scope.addDataSeries = function() {
      console.log($scope.selectedDataSeries);

      var _helper = function(type, target) {
        $scope.buffers[type].some(function(element, index, arr) {
          if (element.id == target.id) {
            arr.splice(index, 1);
            return true;
          }
          return false;
        });
      }

      $scope.selectedsDataSeries.forEach(function(target) {
        $scope.selectedDataSeriesList.push(target);

        if (target.isDynamic) {
          _helper("dynamic", target);
        } else {
          _helper("static", target);
        }
      })

      $scope.selectedsDataSeries = [];
    };

    // it check if there is a dataseries selected
    $scope.isAnyDataSeriesSelected = function() {
      return $scope.selectedDataSeries && $scope.selectedDataSeries.id > 0;
    }

    // it handles hidden box with data-series analysis metadata
    $scope.onDataSeriesClick = function(dataSeries) {
      if (dataSeries.id == $scope.selectedDataSeries.id) {
        $scope.selectedDataSeries = {};
        return;
      }
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

      // sending post operation
      AnalysisFactory.post({
        analysis: analysisToSend,
        storager: storager
      }).success(function(data) {
        window.location = "/configuration/analyses";
      }).error(errorHelper);
    };
  }]);
