define([], function(){
  'use strict';

  var InterpolatorRegisterUpdate = function($scope, $q, $window, $http, $log, $timeout, i18n, MessageBoxService, Service, DataSeriesSemanticsService, DataProviderService, InterpolatorService){

    $scope.MessageBoxService = MessageBoxService;
    $scope.ServiceInstance = Service;
    $scope.InterpolatorService = InterpolatorService;
    $scope.DataSeriesSemanticsService = DataSeriesSemanticsService;
    $scope.DataProviderService = DataProviderService;
    $scope.inter = {};
    $scope.outputDataSeries = {};
    $scope.bounding_rect = {}
    $scope.BASE_URL = BASE_URL;
    $scope.schedule = {};
    $scope.scheduleOptions = {
      showAutomaticOption: true
    };

    var config = $window.configuration;

    var isUpdating = false;
    if (config.interpolator)
      isUpdating = true;

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

        if (isUpdating){
          prepareObjectToUpdate();
        }
      });
    });

    var prepareDataSetFormatToForm = function(fmt){
      var output = {};
      for(var k in fmt) {
        if (fmt.hasOwnProperty(k)) {
          // checking if a number
          if (isNaN(fmt[k]) || fmt[k] == "" || typeof fmt[k] == "boolean") {
            if (k === "active") {
              output[k] = typeof fmt[k] === "string" ? fmt[k] === "true" : fmt[k]; 
            } else {
              output[k] = fmt[k];
            }
          } else {
            output[k] = parseFloat(fmt[k]);
          }
        }
      }
      return output;
    };

    var prepareObjectToUpdate = function(){
      $scope.input_data_series = JSON.parse(config.interpolator.dataSeriesInput);
      $scope.outputDataSeries = JSON.parse(config.interpolator.dataSeriesOutput);
      config.interpolator.bounding_rect = JSON.parse(config.interpolator.bounding_rect);
      var bounding_rect = {
        ll_corner: {
          x: config.interpolator.bounding_rect.ll_corner[0],
          y: config.interpolator.bounding_rect.ll_corner[1]
        },
        ur_corner: {
          x: config.interpolator.bounding_rect.ur_corner[0],
          y: config.interpolator.bounding_rect.ur_corner[1]
        }
      };
      $scope.inter = config.interpolator;
      $scope.inter.name = $scope.outputDataSeries.name;
      $scope.bounding_rect = bounding_rect;
      $scope.inter.description = $scope.outputDataSeries.description;
      $scope.outputDataSeries.data_series_semantics = $scope.DataSeriesSemanticsService.get({code: "GRID-geotiff"});
      $scope.outputDataSeries.data_provider = $scope.DataProviderService.list({id: $scope.outputDataSeries.data_provider_id})[0];
      $scope.outputDataSeries.format = prepareDataSetFormatToForm($scope.outputDataSeries.datasets[0].format);

      $timeout(function() {

        $scope.schedule = {};
        $scope.inter.schedule.scheduleType = $scope.inter.schedule_type.toString();
        $scope.$broadcast("updateSchedule", $scope.inter.schedule || {});

      });
    };
    
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

      var scheduleValues = Object.assign({}, $scope.schedule);
      switch(scheduleValues.scheduleHandler) {
        case "seconds":
        case "minutes":
        case "hours":
          scheduleValues.frequency_unit = scheduleValues.scheduleHandler;
          scheduleValues.frequency_start_time = scheduleValues.frequency_start_time ? moment(scheduleValues.frequency_start_time).format("HH:mm:ssZ") : "";
          break;
        case "weeks":
        case "monthly":
        case "yearly":
          // todo: verify
          var dt = scheduleValues.schedule_time;
          scheduleValues.schedule_unit = scheduleValues.scheduleHandler;
          scheduleValues.schedule_time = moment(dt).format("HH:mm:ss");
          break;
        default:
          break;
      }
    
      var operation = $scope.InterpolatorService.create({interpolator: objectToSave, schedule: scheduleValues});
      operation.then(function(response) {
        $window.location.href = BASE_URL + "configuration/dynamic/dataseries?token=" + response.token;
      }).catch(function(err) {
        $log.info(err);
        $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(err));
      }); 
    }

  };

  InterpolatorRegisterUpdate.$inject = ["$scope", "$q", "$window", "$http", "$log", "$timeout", "i18n", "MessageBoxService", "Service", "DataSeriesSemanticsService", "DataProviderService", "InterpolatorService"];

  return InterpolatorRegisterUpdate;
})