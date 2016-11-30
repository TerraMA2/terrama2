angular.module('terrama2.status', ['terrama2.services', 'terrama2.table', 'terrama2.components.messagebox'])
  .controller('StatusController', ['$scope', '$HttpTimeout', 'Socket', 'i18n', '$window',
  function($scope, $HttpTimeout, Socket, i18n, $window) {

    var Globals = $window.globals;
    $scope.i18n = i18n;
    $scope.globals = Globals;
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = "";
    var cachedIcons = {};
    cachedIcons[Globals.enums.StatusLog.DONE] = "/images/check.png";
    cachedIcons[Globals.enums.StatusLog.ERROR] = "/images/error.png";
    cachedIcons[Globals.enums.StatusLog.DOWNLOADED] = "/images/download.png";
    cachedIcons["message_" +Globals.enums.MessageType.WARNING_MESSAGE] = "/images/warning.png";

    // injecting socket in angular scope
    $scope.socket = Socket;

    $scope.model = [];

    $scope.fields = [
      {
        key: 'type',
        as: i18n.__('Type')
      },
      {
        key: 'service',
        as: i18n.__('Service')
      },
      {
        key: 'name',
        as: i18n.__('Name')
      },
      {
        key: 'message',
        as: i18n.__('Message')
      },
      {
        key: 'date',
        as: i18n.__('Date')
      }
    ];

    $scope.link = function(object) {
      return "";
    };

    $scope.iconProperties = {
      type: "img",
      width: 24,
      height: 24
    };

    $scope.iconFn = function(object) {
      var iconPath = cachedIcons[object.status];
      if (iconPath) {
        // if warning
        if (object.messageType === Globals.enums.MessageType.WARNING_MESSAGE) {
          return cachedIcons["message_" + Globals.enums.MessageType.WARNING_MESSAGE];
        }
        return iconPath;
      }
      return;
    };

    $scope.loading = true;

    $scope.selectedLog = null;
    //Set messages to show in modal
    $scope.setSelectedLog = function(logRow){
      $scope.selectedLog = logRow;
    }

    // socket listeners
    $scope.socket.on('closeResponse', function(response) {
      console.log(response);
      $scope.loading = false;
    });

    $scope.socket.on('errorResponse', function(response) {
      console.log(response);
      $scope.loading = false;

      if (response.service === 0) {
        $scope.alertBox.title = "Status";
        $scope.alertBox.message = $scope.i18n.__("Could not retrieve log. Please check if there a service running in Administration Dashboard");
        $scope.alertLevel = "alert-warning";
        $scope.display = true;
      }
    });

    $scope.socket.on('logResponse', function(response) {
      console.log(response);
      $scope.loading = false;
      var service = response.service;

      var serviceType = response.service_type;
      var targetArray = [];
      var targetMessage = "";
      var targetKey = "";
      switch(serviceType) {
        case Globals.enums.ServiceType.COLLECTOR:
          targetArray = configuration.collectors;
          targetMessage = "Collector";
          targetKey = "dataSeriesOutput";
          break;
        case Globals.enums.ServiceType.ANALYSIS:
          targetArray = configuration.analysis;
          targetMessage = "Analysis ";
          targetKey = "dataSeries";
          break;
        case Globals.enums.ServiceType.VIEW:
          targetArray = configuration.views;
          targetMessage = "View ";
          targetKey = "";
          break;
      }

      var logArray = response.logs;
      var currentOffSet = (new Date().getTimezoneOffset());

      var _findOne = function(array, identifier) {
        var output = {};
        array.some(function(element) {
          if (element.id === identifier) {
            output = element;
            return true;
          }
        })
        return output;
      };

      logArray.forEach(function(logProcess) {
        logProcess.log.forEach(function(logMessage) {
          var out = {
            date: moment(logMessage.last_process_timestamp.split('.')[0], "YYYY-MMM-DD hh:mm:ss").subtract(currentOffSet/60, 'hours'),
            status: logMessage.status,
            type: targetMessage,
            service: service
          };

          var currentProcess = _findOne(targetArray, logProcess.process_id);

          var obj = currentProcess[targetKey] || {name: currentProcess.name};

          out.name = obj.name;
          var messageString = "";
          out.messages = logMessage.messages;
          if (logMessage.messages && logMessage.messages.length > 0){
            var firstMessage = logMessage.messages[0];
            out.message = firstMessage.description;
            out.messageType = firstMessage.type;
          } else {
            var dummyMessage = {};
            switch(logMessage.status) {
              case Globals.enums.StatusLog.DONE:
                dummyMessage.description = "Done";
                dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                break;
              case Globals.enums.StatusLog.START:
                dummyMessage.description = "Started";
                dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                break;
              case Globals.enums.StatusLog.DOWNLOADED:
                dummyMessage.description = "Downloaded";
                dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                break;
              case Globals.enums.StatusLog.ERROR:
                dummyMessage.description = "Error";
                dummyMessage.messageType = Globals.enums.MessageType.ERROR_MESSAGE;
                break;
            }
            out.message = dummyMessage.description;
            out.messages = [dummyMessage];
          }

          $scope.model.push(out)
        })
      })
    });

    $scope.socket.emit('log', {
      begin: 0,
      end: 2
    })

    if(configuration.parameters.message !== undefined && configuration.parameters.message !== null && configuration.parameters.message !== "") {
      $scope.alertBox.title = i18n.__("Project");
      $scope.alertBox.message = configuration.parameters.message;
      $scope.alertLevel = "alert-success";
      $scope.display = true;
    }

  }])
