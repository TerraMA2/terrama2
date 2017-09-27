define([], function(){
  'use strict';

  var InterpolatorRegisterUpdate = function($scope, $q, $http, $log, i18n, MessageBoxService, Service, InterpolatorService){

    $scope.MessageBoxService = MessageBoxService;
    $scope.ServiceInstance = Service;
    $scope.InterpolatorService = InterpolatorService;
    $scope.inter = {};

    $q.all([
      i18n.ensureLocaleIsLoaded(),
    ]).then(function(){
      return $scope.ServiceInstance.init().then(function(){

        $scope.filteredServices = $scope.ServiceInstance.list({'service_type_id': $scope.ServiceInstance.types.INTERPOLATION});
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
    
    $scope.save = function(){
      console.log($scope.inter);
    
/*       var operation = $scope.InterpolatorService.create({interpolator: $scope.interpolator, schedule:{schedule_type: 3}});
      operation.then(function(response) {
        console.log(response);
        $log.info(response);
      }).catch(function(err) {
        $log.info(err);
        $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(err));
      }); */
    }

  };

  InterpolatorRegisterUpdate.$inject = ["$scope", "$q", "$http", "$log", "i18n", "MessageBoxService", "Service","InterpolatorService"];

  return InterpolatorRegisterUpdate;
})