define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/table/table",
  "TerraMA2WebApp/alert-box/app"
],function(commonServiceModule, tableModule, alertBoxModule) {
  "use strict";

  var moduleName = "terrama2.status";

  angular.module(moduleName, [commonServiceModule, tableModule, alertBoxModule])
    .controller('StatusController', ['$scope', '$HttpTimeout', 'Socket', 'i18n', '$window', 'MessageBoxService',
    function($scope, $HttpTimeout, Socket, i18n, $window, MessageBoxService) {
      var Globals = $window.globals;
      var config = $window.configuration;
      $scope.i18n = i18n;
      $scope.globals = Globals;
      $scope.MessageBoxService = MessageBoxService;
      var title = i18n.__("Status");
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
      $scope.close = function() {
        MessageBoxService.reset();
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
        $scope.loading = false;
      });

      $scope.socket.on('errorResponse', function(response) {
        $scope.loading = false;

        if (response.service === 0) {
          return MessageBoxService.warning(title, i18n.__("Could not retrieve log. Please check if there a service running in Administration Dashboard"));
        }
      });

      $scope.socket.on('logResponse', function(response) {
        $scope.loading = false;
        var service = response.service;

        var serviceType = response.service_type;
        var targetArray = [];
        var targetMessage = "";
        var targetKey = "";
        switch(serviceType) {
          case Globals.enums.ServiceType.COLLECTOR:
            targetArray = config.collectors;
            targetMessage = "Collector";
            targetKey = "dataSeriesOutput";
            break;
          case Globals.enums.ServiceType.ANALYSIS:
            targetArray = config.analysis;
            targetMessage = "Analysis ";
            targetKey = "dataSeries";
            break;
          case Globals.enums.ServiceType.VIEW:
            targetArray = config.views;
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

              switch(logMessage.status) {
                case Globals.enums.StatusLog.DONE:
                  out.message = "Done...";
                  out.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
                case Globals.enums.StatusLog.START:
                  out.message = "Started...";
                  out.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
                case Globals.enums.StatusLog.DOWNLOADED:
                  out.message = "Downloaded...";
                  out.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
                case Globals.enums.StatusLog.ERROR:
                  var firstMessage = logMessage.messages[0];
                  out.message = firstMessage.description;
                  out.messageType = firstMessage.type;
                  break;
              }
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
                case Globals.enums.StatusLog.ON_QUEUE:
                  dummyMessage.description = "On queue";
                  dummyMessage.messageType = Globals.enums.MessageType.ERROR_MESSAGE;
                  break;
              }
              out.message = dummyMessage.description;
              out.messages = [dummyMessage];
            }

            $scope.model.push(out);
          });
        });
      });

      $scope.socket.emit('log', {
        begin: 0,
        end: 2
      });

      if(config.parameters.message !== undefined && config.parameters.message !== null && config.parameters.message !== "") {
        MessageBoxService.success(i18n.__("Project"), config.parameters.message);
      }

    }]);
  
  return moduleName;
});
