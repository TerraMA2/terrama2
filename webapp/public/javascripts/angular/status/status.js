angular.module('terrama2.status', ['terrama2.services', 'terrama2.table', 'terrama2.components.messagebox'])
  .controller('StatusController', ['$scope', '$HttpTimeout', 'Socket', 'i18n',
  function($scope, $HttpTimeout, Socket, i18n) {
    $scope.i18n = i18n;
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = "";

    // injecting socket in angular scope
    $scope.socket = Socket;

    $scope.model = [];

    $scope.fields = [
      {
        key: 'message',
        as: 'Message'
      },
      {
        key: 'date',
        as: 'Date'
      }
    ];

    $scope.link = function(object) {
      return "";
    };

    $scope.iconProperties = {
      type: "img",
      width: 16,
      height: 16
    };

    $scope.iconFn = function(object) {
      switch(object.status) {
        case globals.enums.StatusLog.DONE:
          return "/images/check.png";
          break;
        case globals.enums.StatusLog.ERROR:
          return "/images/warning.png";
          break;
      }
    }

    $scope.loading = true;

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

      var serviceType = response.service_type;
      var targetArray = [];
      var targetMessage = "";
      var targetKey = "";
      switch(serviceType) {
        case globals.enums.ServiceType.COLLECTOR:
          targetArray = configuration.collectors;
          targetMessage = "Collector ";
          targetKey = "dataSeriesOutput";
          break;
        case globals.enums.ServiceType.ANALYSIS:
          targetArray = configuration.analysis;
          targetMessage = "Analysis ";
          targetKey = "dataSeries";
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
            date: moment(logMessage.last_process_timestamp.split('.')[0]).subtract(currentOffSet/60, 'hours').calendar(),
            status: logMessage.status
          };

          var currentProcess = _findOne(targetArray, logProcess.process_id);

          var obj = currentProcess[targetKey];

          out.message = targetMessage;

          if (typeof obj === "string") {
            out.message += obj;
          } else {
            out.message += obj.name;
          }

          switch(logMessage.status) {
            case globals.enums.StatusLog.DONE:
              out.message += " - done";
              break;
            case globals.enums.StatusLog.START:
              out.message += " - started";
              break;
            case globals.enums.StatusLog.DOWNLOADED:
              out.message += " - downloaded";
              break;
            case globals.enums.StatusLog.ERROR:
              out.message += " - error";
              break;
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
