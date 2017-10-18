define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/table/table",
  "TerraMA2WebApp/alert-box/app"
],function(commonServiceModule, tableModule, alertBoxModule) {
  "use strict";

  var moduleName = "terrama2.status";

  angular.module(moduleName, [commonServiceModule, tableModule, alertBoxModule])
    .controller('StatusController', ['$scope', '$HttpTimeout', 'Socket', 'i18n', '$window', 'MessageBoxService', '$timeout',
    function($scope, $HttpTimeout, Socket, i18n, $window, MessageBoxService, $timeout) {
      var Globals = $window.globals;
      var config = $window.configuration;
      $scope.logSize = 0;
      $scope.i18n = i18n;
      $scope.globals = Globals;
      $scope.statusPerPage = 10;
      $scope.MessageBoxService = MessageBoxService;
      var title = i18n.__("Status");
      var cachedIcons = {};
      cachedIcons[Globals.enums.StatusLog.DONE] = BASE_URL + "images/status/green.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.DONE] = BASE_URL + "images/status/green_anime.gif";
      cachedIcons[Globals.enums.StatusLog.ERROR] = BASE_URL + "images/status/red.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.ERROR] = BASE_URL + "images/status/red_anime.gif";
      cachedIcons[Globals.enums.StatusLog.DOWNLOADED] = BASE_URL + "images/status/download.png";
      cachedIcons["message_" +Globals.enums.MessageType.WARNING_MESSAGE] = BASE_URL + "images/status/yellow.gif";
      cachedIcons["start_warning"] = BASE_URL + "images/status/yellow_anime.gif";
      cachedIcons[Globals.enums.StatusLog.START] = BASE_URL + "images/status/grey_anime.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.START] = BASE_URL + "images/status/grey_anime.gif";
      cachedIcons[Globals.enums.StatusLog.ON_QUEUE] = BASE_URL + "images/status/clock.png";
      cachedIcons[Globals.enums.StatusLog.INTERRUPTED] = BASE_URL + "images/status/red.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.INTERRUPTED] = BASE_URL + "images/status/red_anime.gif";
      cachedIcons[Globals.enums.StatusLog.NOT_EXECUTED] = BASE_URL + "images/status/grey.gif";
      cachedIcons["start_" + Globals.enums.StatusLog.NOT_EXECUTED] = BASE_URL + "images/status/grey_anime.gif";

      $scope.onPageChanged = function(currentPage, previousPage) {
        // TODO: Paginate dinamically
        // var begin = $scope.statusPerPage * currentPage;
        // Socket.emit("log", {begin: begin, end: begin + $scope.statusPerPage});
      };

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
          key: 'project',
          as: i18n.__('Project')
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
        var nameTypeKey = statusObject.name + statusObject.type;
        if ($scope.groupedModel[nameTypeKey].length > 1){
          var lastObjectStatus = getLastValidStatus($scope.groupedModel[nameTypeKey]); 
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
            targetMessage = "Analysis";
            targetKey = "dataSeries";
            break;
          case Globals.enums.ServiceType.VIEW:
            targetArray = config.views;
            targetMessage = "View";
            targetKey = "";
            break;
          case Globals.enums.ServiceType.ALERT:
            targetArray = config.alerts;
            targetMessage = "Alert";
            targetKey = "";
            break;
          case Globals.enums.ServiceType.INTERPOLATION:
            targetArray = config.interpolators;
            targetMessage = "Interpolator";
            targetKey = "";
            break;
        }

        var logArray = response.logs;

        var _findOne = function(array, identifier) {
          var output = {};
          array.some(function(element) {
            if (element.id === identifier) {
              output = element;
              return true;
            }
          });
          return output;
        };

        //Function to get index of object in array
        var arrayObjectIndexOf = function(myArray, searchObject) {
          for(var i = 0, len = myArray.length; i < len; i++) {
              if (myArray[i]['name'] === searchObject.name && myArray[i]['type'] === targetMessage) return i;
          }
          return -1;
        }

        // Removing logs to be replaced
        logArray.forEach(function(logProcess){
          var currentProcess = _findOne(targetArray, logProcess.process_id);
          if (currentProcess){
            var obj = currentProcess[targetKey] || {name: currentProcess.name};
            var index = arrayObjectIndexOf($scope.model, obj);

            while(index !== -1){
              $scope.model.splice(index, 1);
              index = arrayObjectIndexOf($scope.model, obj);
            }
          }
        });

        logArray.forEach(function(logProcess) {
          $scope.logSize += logProcess.log.length;
          logProcess.log.forEach(function(logMessage) {
            var out = {
              date: moment(logMessage.last_process_timestamp, "YYYY-MM-DDThh:mm:ss.SSSSSSZ"),
              status: logMessage.status,
              type: targetMessage,
              service: service
            };

            var currentProcess = _findOne(targetArray, logProcess.process_id);

            var obj = currentProcess[targetKey] || {name: currentProcess.name};

            var projectName = "";
            var projectId = (currentProcess.class === "Collector" ? currentProcess.dataSeriesOutput.project_id : currentProcess.project_id);

            for(var i = 0, projectsLength = config.projects.length; i < projectsLength; i++) {
              if(projectId === config.projects[i].id) {
                projectName = config.projects[i].name;
                break;
              }
            }

            out.name = obj.name;
            out.project = projectName;
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
                  if (logMessage.data){
                    var dateProcessInfo = getDateProcessInfo(logMessage.data);
                    if (dateProcessInfo){
                      var dateProcessObject = {
                        description: dateProcessInfo,
                        messagetype: Globals.enums.MessageType.INFO_MESSAGE
                      };
                      out.messages.push(dateProcessObject);
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
              var dummyMessages;
              switch(logMessage.status) {
                case Globals.enums.StatusLog.DONE:
                  if (logMessage.data){
                    var dateProcessInfo = getDateProcessInfo(logMessage.data);
                    dummyMessage.description = "Done...";
                    dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                    dummyMessages = {
                      description: dateProcessInfo,
                      messagetype: Globals.enums.MessageType.INFO_MESSAGE
                    };
                    break;
                  } else {
                    dummyMessage.description = "Done";
                    dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                    break;
                  }
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
                case Globals.enums.StatusLog.INTERRUPTED:
                  dummyMessage.description = "Interrupted";
                  dummyMessage.messageType = Globals.enums.MessageType.ERROR_MESSAGE;
                  break;
                case Globals.enums.StatusLog.NOT_EXECUTED:
                  dummyMessage.description = "Not executed";
                  dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
                case Globals.enums.StatusLog.ON_QUEUE:
                  dummyMessage.description = "On queue";
                  dummyMessage.messageType = Globals.enums.MessageType.INFO_MESSAGE;
                  break;
              }
              out.message = dummyMessage.description;
              out.messages = dummyMessages ? [dummyMessages] : [dummyMessage];
            }

            $scope.model.push(out);
            var nameTypeKey = out.name + out.type;
            if ($scope.groupedModel.hasOwnProperty(nameTypeKey)){
              $scope.groupedModel[nameTypeKey].push(out);
            }
            else {
              $scope.groupedModel[nameTypeKey] = [out];
            }
          });
        });
        for (var key in $scope.groupedModel){
          $scope.groupedModel[key] =  $scope.groupedModel[key].sort(function(a,b) {return (a.date > b.date) ? -1 : ((b.date > a.date) ? 1 : 0);} );
        }
      });

      // Function to get processing time message
      function getDateProcessInfo(stringDate){
        var dateJson = JSON.parse(stringDate);
        var startTime = moment(dateJson.processing_start_time[0]);
        var endTime = moment(dateJson.processing_end_time[0]);
        var format = "HH[h] mm[m] ss[s]";
        var difference = moment.utc(endTime.diff(startTime));
        if (difference < 60000){
          format = "ss[s]";
        } else if (difference < 3600000){
          format = "mm[m] ss[s]";
        }
        var message;
        if (difference.isValid())
          var message = i18n.__("Processing time") + ": " + difference.format(format);
        return message;
      }

      $scope.socket.emit('log', {});

      if(config.parameters.message !== undefined && config.parameters.message !== null && config.parameters.message !== "") {
        var messageArray = config.parameters.message.split(" ");
        var tokenCodeMessage = messageArray[messageArray.length - 1];
        messageArray.splice(messageArray.length - 1, 1);

        $timeout(function() {
          var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
          MessageBoxService.success(i18n.__("Project"), finalMessage);
        }, 1000);
      }
    }]);
  
  return moduleName;
});
