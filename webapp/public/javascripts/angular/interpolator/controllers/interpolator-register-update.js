define([], function(){
  'use strict';

  var InterpolatorRegisterUpdate = function($scope, $q, $window, $http, $log, $timeout, i18n, MessageBoxService, Service, DataSeriesSemanticsService, DataProviderService, InterpolatorService, Socket){

    $scope.i18n = i18n;
    $scope.MessageBoxService = MessageBoxService;
    $scope.ServiceInstance = Service;
    $scope.InterpolatorService = InterpolatorService;
    $scope.DataSeriesSemanticsService = DataSeriesSemanticsService;
    $scope.DataProviderService = DataProviderService;
    $scope.BASE_URL = BASE_URL;

    var config = $window.configuration;
    $scope.inter = {};
    $scope.outputDataSeries = {};
    $scope.bounding_rect = {}
    $scope.scheduleOptions = {
      showAutomaticOption: true
    };
    $scope.cssBoxSolid = {
      boxType: "box-solid"
    };

    $scope.timezoneOptions = [];
    var timezoneOption = -12;
    while (timezoneOption <= 12){
      $scope.timezoneOptions.push(timezoneOption);
      timezoneOption++;
    }
    $scope.forms = {};

    var isUpdating = false;
    if (config.interpolator)
      isUpdating = true;

    if (config.input_ds)
      $scope.input_data_series = config.input_ds;

    function closeDialog() {
      $scope.MessageBoxService.reset();
    }
    $scope.close = closeDialog;

    $scope.addTifExtention = function(){
      if (!$scope.outputDataSeries.format.mask.endsWith(".tif")){
        if ($scope.outputDataSeries.format.mask.indexOf(".") > -1){
          $scope.outputDataSeries.format.mask = $scope.outputDataSeries.format.mask.slice(0, $scope.outputDataSeries.format.mask.indexOf("."));
        }
        $scope.outputDataSeries.format.mask += ".tif";
      }
    };

    $q.all([
      i18n.ensureLocaleIsLoaded(),
      $scope.DataSeriesSemanticsService.init(),
      $scope.DataProviderService.init(),
    ]).then(function(){
      return $scope.ServiceInstance.init().then(function(){
        $scope.filteredServices = $scope.ServiceInstance.list({'service_type_id': $scope.ServiceInstance.types.INTERPOLATION});
        $scope.storagerFormats = $scope.DataSeriesSemanticsService.list({code: "GRID-geotiff"});
        $scope.providersList = $scope.DataProviderService.list();
        $scope.providers = [];
        $scope.providersList.forEach(function(dataProvider) {
          $scope.storagerFormats[0].data_providers_semantics.forEach(function(demand) {
            if(dataProvider.data_provider_type.id == demand.data_provider_type_id) {
              $scope.providers.push(dataProvider);
            }
          })
        });
    
        if (isUpdating){
          prepareObjectToUpdate();
        } else {
          $scope.outputDataSeries.data_series_semantics = $scope.storagerFormats[0];
          $scope.outputDataSeries.data_provider = $scope.providers[0];
          $scope.inter.service_instance_id = $scope.filteredServices[0].id;
          $scope.inter.active = true;
        }
      });
    });

    // Flag to verify if can not save if the service is not running
    var canSave = true;
    var serviceOfflineMessage = "If service is not running you can not save the interpolator. Start the service before create or update an interpolator!";
    // Watch service select, to check status
    $scope.$watch("inter.service_instance_id", function(service_id) {
      if (service_id)
        Socket.emit('status', {service: service_id});
    }, true);

    Socket.on('statusResponse', function(response){
      if(response.service == $scope.inter.service_instance_id){
        if (response.checking === undefined || (!response.checking && response.status === 400)) {
          if (!response.online){
            $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(serviceOfflineMessage));
            canSave = false;
          } else {
            canSave = true;
          }
        }
      }
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
      $scope.input_data_series = config.interpolator.dataSeriesInput;
      $scope.outputDataSeries = config.interpolator.dataSeriesOutput;
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
      $scope.inter.interpolator_strategy = $scope.inter.interpolator_strategy;
      $scope.bounding_rect = bounding_rect;
      $scope.inter.description = $scope.outputDataSeries.description;
      $scope.outputDataSeries.data_series_semantics = $scope.DataSeriesSemanticsService.get({code: "GRID-geotiff"});
      $scope.outputDataSeries.data_provider = $scope.DataProviderService.list({id: $scope.outputDataSeries.data_provider_id})[0];
      $scope.outputDataSeries.format = prepareDataSetFormatToForm($scope.outputDataSeries.datasets[0].format);

      $timeout(function() {
        $scope.inter.schedule.scheduleType = $scope.inter.schedule_type.toString();
        $scope.$broadcast("updateSchedule", $scope.inter.schedule || {});
      });
    };
    
    var prepareObjectToSave = function(){
      var outputDataSeries = {};
      var outputDataSet = {};
      if (isUpdating){
        outputDataSet = Object.assign({}, $scope.outputDataSeries.datasets[0]);
        outputDataSet.format = $scope.outputDataSeries.format;
        outputDataSeries = Object.assign({}, $scope.outputDataSeries);
        outputDataSeries.name = $scope.inter.name;
        outputDataSeries.description = $scope.inter.description;
        outputDataSeries.data_series_semantics_id = $scope.outputDataSeries.data_series_semantics.id,
        outputDataSeries.active = $scope.inter.active;
        outputDataSeries.dataSets = [outputDataSet];
      } else {
        var outputDataSet = {
          active: $scope.inter.active,
          format: $scope.outputDataSeries.format
        }
        var outputDataSeries = {
          name: $scope.inter.name,
          data_provider_id: $scope.outputDataSeries.data_provider.id,
          data_series_semantics_id: $scope.outputDataSeries.data_series_semantics.id,
          description: $scope.inter.description,
          dataSets: [outputDataSet],
          active: $scope.inter.active,
        }
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
    };

    $scope.save = function(shouldRun){
      // broadcasting each one terrama2 field directive validation
      $scope.$broadcast("formFieldValidation");

      if ($scope.forms.generalDataForm.$invalid ||
        $scope.forms.parameterDataForm.$invalid ||
        $scope.forms.storeDataForm.$invalid) {
          $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__("There are invalid fields on form"));
          return;
      }

      if(!canSave){
        $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(serviceOfflineMessage));
        return;
      }

      var objectToSave = prepareObjectToSave();

      var scheduleValues = Object.assign({}, $scope.inter.schedule);
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
          if (scheduleValues.scheduleType == "4"){
            scheduleValues.data_ids = [$scope.input_data_series.id];
          }
          break;
      }

      objectToSave.run = shouldRun;
    
      var operation = isUpdating ? 
                      $scope.InterpolatorService.update( $scope.inter.id, {interpolator: objectToSave, schedule: scheduleValues}) : 
                      $scope.InterpolatorService.create({interpolator: objectToSave, schedule: scheduleValues});
      operation.then(function(response) {
        $window.location.href = BASE_URL + "configuration/dynamic/dataseries?token=" + response.token;
      }).catch(function(err) {
        $log.info(err);
        $scope.MessageBoxService.danger(i18n.__("Interpolator"), i18n.__(err));
      });
    }

  };

  InterpolatorRegisterUpdate.$inject = ["$scope", "$q", "$window", "$http", "$log", "$timeout", "i18n", "MessageBoxService", "Service", "DataSeriesSemanticsService", "DataProviderService", "InterpolatorService", "Socket"];

  return InterpolatorRegisterUpdate;
})