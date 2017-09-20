define([], function() {
  function ListController($scope, $http, AnalysisService, MessageBoxService, Socket, i18n, $log, Service, $window, $timeout) {
    var config = $window.configuration;
    var globals = $window.globals;
    $scope.model = [];
    $scope.i18n = i18n;
    $scope.disabledButtons = {};
    $scope.MessageBoxService = MessageBoxService;

    var serviceCache = {};

    Service.init();
    var title = "Analysis";

    Socket.on('errorResponse', function(response){
      var errorMessage = response.message;
      var targetMethod = null;
      if (serviceCache[response.service] != undefined){
        var service = Service.get(serviceCache[response.service].process_ids.service_instance);
        if(service != null) {
          errorMessage = i18n.__(errorMessage) + " " + i18n.__("Service") + ": '" + service.name + "' ";
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
      targetMethod.call(MessageBoxService, i18n.__(title), errorMessage);
      delete serviceCache[response.service];
    });

    Socket.on('runResponse', function(response){
      MessageBoxService.success(i18n.__(title), i18n.__("The process was started successfully"));
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

    var restriction = {
      project_id: config.projectId
    };

    $scope.iconProperties = {
      type: "img"
    };

    $scope.fields = [
      {key: "name", as: i18n.__("Name")},
      {key: "type.name", as: i18n.__("Type")},
      {key: "description", as: i18n.__("Description")}
    ];

    $scope.linkToAdd = BASE_URL + "configuration/analysis/new";

    $scope.link = function(object) {
      return BASE_URL + "configuration/analysis/" + object.id + "/edit";
    };

    if(config.message !== "") {
      var messageArray = config.message.split(" ");
      var tokenCodeMessage = messageArray[messageArray.length - 1];
      messageArray.splice(messageArray.length - 1, 1);

      $timeout(function() {
        var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
        if(!MessageBoxService.alertBox.display) MessageBoxService.success(i18n.__(title), finalMessage);
      }, 1000);
    }

    $scope.extra = {
      canRemove: config.hasProjectPermission,
      advancedFilters: [
        {
          name: "Grid",
          value: "Grid",
          checked: true
        },
        {
          name: "Monitored Object",
          value: "Monitored Object",
          checked: true
        },
        {
          name: "DCP",
          value: "Dcp",
          checked: true
        }
      ],
      advancedFilterField: "type.name",
      removeOperationCallback: function(err, data) {
        if (err) {
          if (err.serviceStoppedError){
            var errorWhenDeleteMessage = i18n.__("Can not delete the analysis if the service is not running. ");
            if (err.service && err.service.instance_name)
              errorWhenDeleteMessage += i18n.__("Service") + ": " + err.service.instance_name;
            return MessageBoxService.danger(i18n.__(title), errorWhenDeleteMessage);            
          } else {
            MessageBoxService.danger(i18n.__(title), i18n.__(err.message));
            return;
          }
        }
        MessageBoxService.success(i18n.__(title), data.name + i18n.__(" removed"));
      },
      showRunButton: true,
      canRun: function(object){
        return true;
      },
      run: function(object){
        serviceCache[object.service_instance_id] = {
          "process_ids": {
            "ids":[object.id],
            "service_instance": object.service_instance_id
          },
          "service_id": object.id,
          "service_name": object.name
        };

        $scope.disabledButtons[object.id] = true;

        Socket.emit('status', {service: object.service_instance_id});
      },
      disabledButtons: function(object){
        return $scope.disabledButtons[object.id];
      }
    };

    $scope.remove = function(object) {
      return BASE_URL + "api/Analysis/" + object.id + "/delete";
    };

    $scope.close = function() {
      MessageBoxService.reset();
    };

    $scope.icon = function(object) {
      switch(object.type.id) {
        case globals.enums.AnalysisType.MONITORED:
          return BASE_URL + "images/analysis/monitored-object/monitored-object_analysis.png";
        case globals.enums.AnalysisType.GRID:
          return BASE_URL + "images/analysis/grid/grid_analysis.png";
        default:
          return BASE_URL + "images/analysis/dcp/dcp_analysis.png";
      }
    };

    AnalysisService.init(restriction)
      .then(function(analysis) {
        $scope.model = analysis;

        $timeout(function() {
          $scope.model.forEach(function(instance) {
            instance.type.name = i18n.__(instance.type.name);
          });
        }, 500);

        if(config.extra && config.extra.id) {
          var analysisToRun = $scope.model.filter(function(element) {
            return element.id == config.extra.id;
          });

          if(analysisToRun.length == 1) {
            $scope.extra.run(analysisToRun[0]);
          }
        }
      }).catch(function(err) {
        $log.info("Err in retrieving Analysis " + err);
      }).finally(function() {
        $scope.loading = false;
      });
  }

  ListController.$inject = ["$scope", "$http", "AnalysisService", "MessageBoxService", "Socket", "i18n", "$log", "Service", "$window", "$timeout"];

  return ListController;
});