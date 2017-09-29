define([], function() {

  function ListController($scope, DataSeriesService, Socket, i18n, $window, Service, MessageBoxService, $timeout) {
    $scope.i18n = i18n;
    $scope.disabledButtons = {};
    $scope.orderBy = "name";
    $scope.MessageBoxService = MessageBoxService;
    var isDynamic = false;
    var title = "Data Series";
    var queryParams = {};

    var serviceCache = {};

    var config = $window.configuration;

    var findCollectorAnalysisOrInterpolator = function(dataSeries){
      if (config.dataSeriesType != 'static'){
        var foundCollector = config.collectors.find(function(collector){
          return collector.output_data_series == dataSeries.id;
        });
        var foundAnalysis = config.analysis.find(function(analysi){
          return analysi.dataSeries.id == dataSeries.id;
        });
        var foundInterpolator = config.interpolators.find(function(interpolator){
          return interpolator.data_series_output == dataSeries.id;
        });
        return foundCollector || foundAnalysis || foundInterpolator;
      } else 
        return false;
    }

    Service.init();

    Socket.on('errorResponse', function(response) {
      var errorMessage = response.message;
      var targetMethod = null;
      if(serviceCache[response.service] != undefined) {
        var service = Service.get(serviceCache[response.service].process_ids.service_instance);
        if(service != null) {
          errorMessage = i18n.__(errorMessage) + " " + i18n.__("Service") + ": '" + service.name + "' ";
        }

        if(config.extra && config.extra.id) {
          var warningMessage = config.message + ". ";
          errorMessage = warningMessage + errorMessage;
          targetMethod = MessageBoxService.warning; // setting warning method to display message
          delete config.extra;
          delete config.message;
        } else {
          targetMethod = MessageBoxService.danger;
        }
      }
      targetMethod.call(MessageBoxService, i18n.__(title), errorMessage);
      delete serviceCache[response.service];
    });

    Socket.on('runResponse', function(response){
      var message = "";
      if (config.extra && config.extra.id){
        message = config.message + i18n.__(". The process was started successfully");
      } else {
        message = i18n.__("The process was started successfully");
      }
      MessageBoxService.success(i18n.__(title), message);
    });

    Socket.on('statusResponse', function(response) {
      if(response.checking === undefined || (!response.checking && response.status == 400)) {
        if(response.online) {
          Socket.emit('run', serviceCache[response.service].process_ids);
          delete $scope.disabledButtons[serviceCache[response.service].service_id];
          delete serviceCache[response.service];
        } else {
          delete $scope.disabledButtons[serviceCache[response.service].service_id];
        }
      }
    });

    if (config.dataSeriesType == "static") {
      $scope.dataSeriesType = config.dataSeriesType;
    } else {
      $scope.dataSeriesType = 'dynamic';
      queryParams['collector'] = true;
    }

    queryParams['type'] = $scope.dataSeriesType;

    $scope.model = [];
    $scope.fields = [];

    $scope.remove = function(object) {
      return BASE_URL + "api/DataSeries/" + object.id + "/delete";
    };

    $scope.extra = {
      canInterpolate: function(object){
        if (object.data_series_semantics.data_series_type_name == "DCP"){
          return true;
        }
        return false;
      },
      linkToInterpolate: function(object){
        return urlToInterpolate = BASE_URL + "configuration/interpolator/new/" + object.id;
      },
      canRemove: config.hasProjectPermission,
      removeOperationCallback: function(err, data) {
        if (err) {
          if (err.serviceStoppedError){
            var errorWhenDeleteMessage = i18n.__("Can not delete the data series if the service is not running. ");
            if(err.service && err.service.instance_name)
              errorWhenDeleteMessage += i18n.__("Service") + ": " + err.service.instance_name;
            return MessageBoxService.danger(i18n.__(title), errorWhenDeleteMessage);            
          } else 
            return MessageBoxService.danger(i18n.__(title), err.message);
        }
        MessageBoxService.success(i18n.__(title), data.name + i18n.__(" removed"));
      },
      showRunButton: config.showRunButton,
      canRun: findCollectorAnalysisOrInterpolator,
      run: function(object){
        var service_instance = this.canRun(object);

        serviceCache[service_instance.service_instance_id] = {
          "process_ids": {
            "ids": [service_instance.id],
            "service_instance": service_instance.service_instance_id
          },
          "service_id": object.id,
          "service_name": object.name
        };

        $scope.disabledButtons[object.id] = true;
        Socket.emit('status', {service: service_instance.service_instance_id});
      },
      disabledButtons: function(object){
        return $scope.disabledButtons[object.id];
      }
    };

    if(config.dataSeriesType == "static") {
      $scope.extra.advancedFilters = [
        {
          name: "Geometric Object",
          value: "Geometric Object",
          checked: true
        },
        {
          name: "Grid",
          value: "Grid",
          checked: true
        }
      ];

      $scope.extra.advancedFilterField = "model_type";
    } else {
      $scope.extra.advancedFilters = [
        {
          name: "Analysis",
          value: "Analysis",
          checked: true
        },
        {
          name: "Collector",
          value: "Collector",
          checked: true
        },
        {
          name: "Direct Access",
          value: "Direct Access",
          checked: true
        }
      ];

      $scope.extra.executeAdvancedFilter = function() {
        for(var i = 0, advancedFiltersLength = $scope.extra.advancedFilters.length; i < advancedFiltersLength; i++) {
          for(var j = 0, modelLength = $scope.model.length; j < modelLength; j++) {
            var semantics = $scope.model[j].data_series_semantics_code;
            var type;

            switch(semantics) {
              case "ANALYSIS_MONITORED_OBJECT-postgis":
                type = i18n.__("Analysis");
                break;

              case "DCP-toa5":
              case "DCP-generic":
              case "DCP-inpe":
              case "DCP-postgis":
              case "Occurrence-generic":
              case "OCCURRENCE-wfp":
              case "OCCURRENCE-lightning":
              case "OCCURRENCE-postgis":
                if($scope.extra.canRun($scope.model[j]))
                  type = i18n.__("Collector");
                else
                  type = i18n.__("Direct Access");
                break;

              case "GRID-gdal":
              case "GRID-geotiff":
              case "GRID-ascii":
              case "GRID-grads":
              case "GRID-grib":
                if($scope.model[j].isAnalysis)
                  type = i18n.__("Analysis");
                else {
                  if($scope.extra.canRun($scope.model[j]))
                    type = i18n.__("Collector");
                  else
                    type = i18n.__("Direct Access");
                }
                break;

              default:
                if($scope.extra.canRun($scope.model[j]))
                  type = i18n.__("Collector");
                else
                  type = i18n.__("Direct Access");
                break;
            }

            if(i18n.__($scope.extra.advancedFilters[i].value) === type) {
              if($scope.extra.advancedFilters[i].checked)
                $scope.model[j].showInTable = true;
              else
                $scope.model[j].showInTable = false;
            }
          }
        }
      };
    }

    if(config.message) {
      var messageArray = config.message.split(" ");
      var tokenCodeMessage = messageArray[messageArray.length - 1];
      messageArray.splice(messageArray.length - 1, 1);

      $timeout(function() {
        var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
        if(!MessageBoxService.alertBox.display) MessageBoxService.success(i18n.__(title), finalMessage);
      }, 1000);
    }

    $scope.close = function() { MessageBoxService.reset(); };

    DataSeriesService.init(queryParams).then(function(data) {
      $scope.model = data instanceof Array ? data : [];

      $timeout(function() {
        // processing type
        $scope.model.forEach(function(instance) {
          var value;
          switch(instance.data_series_semantics.data_series_type_name) {
            case globals.enums.DataSeriesType.DCP:
              value = i18n.__("DCP");
              break;
            case globals.enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
              value = i18n.__("Monitored object");
              break;
            case globals.enums.DataSeriesType.OCCURRENCE:
              value = i18n.__("Occurrence");
              break;
            case globals.enums.DataSeriesType.GRID:
              if (instance.isAnalysis) {
                value = i18n.__("Analysis Grid");
              } else {
                value = i18n.__("Grid");
              }
              break;
            case globals.enums.DataSeriesType.GEOMETRIC_OBJECT:
              value = i18n.__("Geometric Object");
              break;
            default:
              value = instance.data_series_semantics.name;
          }

          instance.model_type = value;
          var service_instance = findCollectorAnalysisOrInterpolator(instance);
          instance.service_instance_id = service_instance ? service_instance.service_instance_id : undefined;
        });
      }, 500);

      if (config.extra && config.extra.id){
        var dataSeriesToRun = $scope.model.filter(function(element){
          return element.id == config.extra.id;
        });
        if (dataSeriesToRun.length == 1){
          $scope.extra.run(dataSeriesToRun[0]);
        }
      }
      $scope.fields = [{key: 'name', as: i18n.__("Name")}, {key: "model_type", as: i18n.__("Type")}, {key: "description", as: i18n.__("Description")}];
    });

    $scope.link = config.link || null;

    $scope.linkToAdd = config.linkToAdd || null;

    $scope.iconFn = config.iconFn || null;

    $scope.iconProperties = config.iconProperties || {};
  }

  ListController.$inject = ["$scope", "DataSeriesService", "Socket", "i18n", "$window", "Service", "MessageBoxService", "$timeout"];

  return ListController;
});
