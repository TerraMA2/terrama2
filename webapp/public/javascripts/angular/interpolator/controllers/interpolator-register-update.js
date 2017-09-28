define([], function(){
  'use strict';

  var InterpolatorRegisterUpdate = function($scope, $q, $window, $http, $log, i18n, MessageBoxService, Service, DataSeriesSemanticsService, DataProviderService, InterpolatorService){

    $scope.MessageBoxService = MessageBoxService;
    $scope.ServiceInstance = Service;
    $scope.InterpolatorService = InterpolatorService;
    $scope.DataSeriesSemanticsService = DataSeriesSemanticsService;
    $scope.DataProviderService = DataProviderService;
    $scope.inter = {};
    $scope.outputDataSeries = {};
    $scope.bounding_rect = {}
    $scope.BASE_URL = BASE_URL;

    var config = $window.configuration;

    if (config.input_ds)
      $scope.input_data_series = config.input_ds;

    $q.all([
      i18n.ensureLocaleIsLoaded(),
      $scope.DataSeriesSemanticsService.init(),
      $scope.DataProviderService.init(),
    ]).then(function(){
      return $scope.ServiceInstance.init().then(function(){
        $scope.filteredServices = $scope.ServiceInstance.list({'service_type_id': $scope.ServiceInstance.types.INTERPOLATION});
        $scope.storagerFormats = $scope.DataSeriesSemanticsService.list({code: "GRID-geotiff"});
        $scope.providers = $scope.DataProviderService.list();
      });
    });

    /* $scope.interpolator = {
      active: true,
      service_instance_id: 5,
      data_series_input: 1,
      data_series_output: {
        name: "Tiff do interpolador",
        dataSets: [
          {
            active: true,
            format: {
              mask: "mascara.tif",
              srid: "2442",
              timestamp_property: "file_timestamp",
              timezone: "5"
            },
            mask: "mascara.tif",
            srid: "2442",
            timestamp_property: "file_timestamp",
            timezone: "5"
          }
        ],
        data_provider_id: 1,
        data_series_semantics_id: 8,
        description: undefined,
      },
      schedule_type: 3,
      bounding_rect: {
        "ll_corner": [
          -46.79,
          -24.174
        ],
        "ur_corner": [
            -44.85,
            -23.355
        ]
      },
      interpolation_attribute: "pluvio",
      interpolator_strategy: 2,
      resolution_x: 0.11,
      resolution_y: 0.11,
      srid: 4326,
      metadata: {
        number_of_neighbors: 2,
        power_factor: 3
      }
    }; */
    
    var prepareObjectToSave = function(){
      var outputDataSet = {
        active: $scope.inter.active,
        format: $scope.outputDataSeries.format
      }
      var outputDataSeries = {
        name: $scope.inter.name,
        data_provider_id: $scope.outputDataSeries.data_provider.id,
        data_series_semantics_id: $scope.outputDataSeries.data_series_semantics.id,
        description: $scope.inter.description,
        dataSets: [outputDataSet]
      }
      var interpolator = Object.assign({}, $scope.inter);
      var bounding_rect = {
        ll_corner: [$scope.bounding_rect.ll_corner.x, $scope.bounding_rect.ll_corner.y],
        ur_corner: [$scope.bounding_rect.ur_corner.x, $scope.bounding_rect.ur_corner.y]
      };
      interpolator.bounding_rect = JSON.stringify(bounding_rect);
      interpolator.data_series_output = outputDataSeries;
      interpolator.data_series_input = $scope.input_data_series.id;
      return interpolator;
    }

    $scope.save = function(){
      var objectToSave = prepareObjectToSave();
    
      var operation = $scope.InterpolatorService.create({interpolator: objectToSave, schedule:{schedule_type: 3}});
      operation.then(function(response) {
        console.log(response);
        $log.info(response);
      }).catch(function(err) {
        $log.info(err);
        $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(err));
      }); 
    }

  };

  InterpolatorRegisterUpdate.$inject = ["$scope", "$q", "$window", "$http", "$log", "i18n", "MessageBoxService", "Service", "DataSeriesSemanticsService", "DataProviderService", "InterpolatorService"];

  return InterpolatorRegisterUpdate;
})