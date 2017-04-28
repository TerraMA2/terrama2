define([], function() {

  function ListController($scope, DataSeriesService, Socket, i18n, $window, Service, MessageBoxService) {
    $scope.i18n = i18n;
    $scope.disabledButtons = {};
    $scope.orderBy = "name";
    $scope.MessageBoxService = MessageBoxService;
    var isDynamic = false;
    var title = i18n.__("Data Series");
    var queryParams = {};

    var serviceCache = {};

    var config = $window.configuration;

    Service.init();

    Socket.on('errorResponse', function(response){
      var errorMessage = response.message;
      var targetMethod = null;
      if (serviceCache[response.service] != undefined){
        var service = Service.get(serviceCache[response.service].process_ids.service_instance);
        if(service != null) {
          errorMessage = errorMessage + " " + i18n.__("Service") + ": '" + service.name + "' ";
        }
        if (config.extra && config.extra.id){
          var warningMessage = config.message + ". ";
          errorMessage = warningMessage + errorMessage;
          targetMethod = MessageBoxService.warning; // setting warning method to display message
          delete config.extra;
          delete config.message;
        } 
        else {
          targetMethod = MessageBoxService.danger;
        }
      }
      targetMethod.call(MessageBoxService, title, errorMessage);
      delete serviceCache[response.service];
    });

    Socket.on('runResponse', function(response){
      var message = "";
      if (config.extra && config.extra.id){
        message = config.message + i18n.__(". The process was started successfully");
      } else {
        message = i18n.__("The process was started successfully");
      }
      MessageBoxService.success(title, message);
    });

    Socket.on('statusResponse', function(response) {
      if(response.checking === undefined || (!response.checking && response.status == 400)) {
        if(response.online) {
          Socket.emit('run', serviceCache[response.service].process_ids);
          delete serviceCache[response.service];
        } 
        delete $scope.disabledButtons[serviceCache[response.service].service_id];
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
      return "/api/DataSeries/" + object.id + "/delete";
    };

    $scope.extra = {
      removeOperationCallback: function(err, data) {
        if (err) {
          return MessageBoxService.danger(title, err.message);
        }
        MessageBoxService.success(title, data.name + i18n.__(" removed"));
      },
      showRunButton: config.showRunButton,
      canRun: function(object){
        var foundCollector = config.collectors.find(function(collector){
          return collector.output_data_series == object.id;
        });
        var foundAnalysis = config.analysis.find(function(analysi){
          return analysi.dataSeries.id == object.id;
        })
        return foundCollector || foundAnalysis;
      },
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
    if (config.message) {
      MessageBoxService.success(title, config.message);
    }

    $scope.close = function() { MessageBoxService.reset(); };

    DataSeriesService.init(queryParams).then(function(data) {
      $scope.model = data instanceof Array ? data : [];

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
      });

      if (config.extra && config.extra.id){
        var dataSeriesToRun = $scope.model.filter(function(element){
          return element.id == config.extra.id;
        });
        if (dataSeriesToRun.length == 1){
          $scope.extra.run(dataSeriesToRun[0]);
        }
      }
      $scope.fields = [{key: 'name', as: i18n.__("Name")}, {key: "model_type", as: i18n.__("Type")}];
    });

    $scope.link = config.link || null;

    $scope.linkToAdd = config.linkToAdd || null;

    $scope.iconFn = config.iconFn || null;

    $scope.iconProperties = config.iconProperties || {};
  }

  ListController.$inject = ["$scope", "DataSeriesService", "Socket", "i18n", "$window", "Service", "MessageBoxService"];

  return ListController;
});
