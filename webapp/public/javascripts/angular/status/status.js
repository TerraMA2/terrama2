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
      cachedIcons[Globals.enums.StatusLog.DONE] = "/images/green.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.DONE] = "/images/green_anime.gif";
      cachedIcons[Globals.enums.StatusLog.ERROR] = "/images/red.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.ERROR] = "/images/red_anime.gif";
      cachedIcons[Globals.enums.StatusLog.DOWNLOADED] = "/images/download.png";
      cachedIcons["message_" +Globals.enums.MessageType.WARNING_MESSAGE] = "/images/yellow.gif";
      cachedIcons["start_warning"] = "/images/yellow_anime.png";
      cachedIcons[Globals.enums.StatusLog.START] = "/images/grey_anime.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.START] = "/images/grey_anime.gif";
      cachedIcons[Globals.enums.StatusLog.ON_QUEUE] = "/images/clock.png";

      // injecting socket in angular scope
      $scope.socket = Socket;

      $scope.model = [];
      $scope.groupedModel = {};

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
        if (object.status !== Globals.enums.StatusLog.START){
          var iconPath = cachedIcons[object.status];
          if (iconPath) {
            // if warning
            if (object.messageType === Globals.enums.MessageType.WARNING_MESSAGE && object.status !== Globals.enums.StatusLog.ERROR) {
              return cachedIcons["message_" + Globals.enums.MessageType.WARNING_MESSAGE];
            }
            return iconPath;
          }
          return;
        } else {
          var lastStatus = getStatusKey(object);
          return cachedIcons[lastStatus];
        }
      };

      // Function to get key icon of cached icons based in last service execution
      var getStatusKey = function(statusObject) {
        if ($scope.groupedModel[statusObject.name].length > 1){
          var lastObjectStatus = getLastValidStatus($scope.groupedModel[statusObject.name]); 
          if (lastObjectStatus.messageType === Globals.enums.MessageType.WARNING_MESSAGE && lastObjectStatus.status !== Globals.enums.StatusLog.ERROR){
            return "start_warning";
          } else {
            return "start_" + lastObjectStatus.status;
          }
        } else {
          return statusObject.status;
        }
      }

      // Function to get the last status that is not Start
      var getLastValidStatus = function(statusObjectList){
        var lastValidStatus = statusObjectList[1];
        if (statusObjectList.length > 2){
          if (statusObjectList[1].status === Globals.enums.StatusLog.START){
            lastValidStatus = statusObjectList[2];
          }
        }
        return lastValidStatus;
      }

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

      Socket.on("processFinished", handleProcessFinished);

      function handleProcessFinished(response) {
        $scope.socket.emit('log', {
          begin: 0,
          end: 2
        });
        MessageBoxService.info(i18n.__("Process"), i18n.__("Process") + " " + response.name + " " + i18n.__("finished"));
      }

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
                  var targetType = Globals.enums.MessageType.INFO_MESSAGE;
                  for(var i = 0; i < logMessage.messages.length; ++i) {
                    if (logMessage.messages[i].type === Globals.enums.MessageType.WARNING_MESSAGE) {
                      targetType = Globals.enums.MessageType.WARNING_MESSAGE;
                      break;
                    }
                  }
                  out.messageType = targetType;
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
                  dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
              }
              out.message = dummyMessage.description;
              out.messages = [dummyMessage];
            }

            $scope.model.push(out);
            if ($scope.groupedModel.hasOwnProperty(out.name)){
              $scope.groupedModel[out.name].push(out);
            }
            else {
              $scope.groupedModel[out.name] = [out];
            }
          });
        });
        for (var key in $scope.groupedModel){
          $scope.groupedModel[key] =  $scope.groupedModel[key].sort(function(a,b) {return (a.date > b.date) ? -1 : ((b.date > a.date) ? 1 : 0);} );
        }
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
