angular.module('terrama2.status', ['terrama2.services', 'terrama2.table', 'terrama2.components.messagebox'])
  .controller('StatusController', ['$scope', '$HttpTimeout', 'Socket', 'i18n',
  function($scope, $HttpTimeout, Socket, i18n) {
    $scope.i18n = i18n;
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = "";
    var cachedIcons = {};
    cachedIcons[globals.enums.StatusLog.DONE] = "/images/check.png";
    cachedIcons[globals.enums.StatusLog.ERROR] = "/images/warning.png";
    cachedIcons[globals.enums.StatusLog.DOWNLOADED] = "/images/download.png";

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
      return cachedIcons[object.status];
    };

    $scope.loading = true;

    // modal info
    $scope.modalMessages = "";
    //Set messages to show in modal
    $scope.setModalMessages = function(messages){
      $scope.modalMessages = messages;
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
        case globals.enums.ServiceType.COLLECTOR:
          targetArray = configuration.collectors;
          targetMessage = "Collector";
          targetKey = "dataSeriesOutput";
          break;
        case globals.enums.ServiceType.ANALYSIS:
          targetArray = configuration.analysis;
          targetMessage = "Analysis ";
          targetKey = "dataSeries";
          break;
        case globals.enums.ServiceType.VIEW:
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
          if (logMessage.messages && logMessage.messages.length > 0){
            for (message in logMessage.messages){
              if (logMessage.messages[message].description)
                messageString += logMessage.messages[message].description + ". ";
            }
            out.message = messageString;
          }
          else{
            switch(logMessage.status) {
              case globals.enums.StatusLog.DONE:
                out.message = "Done";
                break;
              case globals.enums.StatusLog.START:
                out.message = "Started";
                break;
              case globals.enums.StatusLog.DOWNLOADED:
                out.message = "Downloaded";
                break;
              case globals.enums.StatusLog.ERROR:
                out.message = "Error";
                break;
            }
          }

          $scope.model.push(out)
        })
      })
    });

    $scope.socket.emit('log', {
      begin: 0,
      end: 2
    })

  }])
