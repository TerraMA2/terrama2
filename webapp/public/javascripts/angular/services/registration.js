angular.module('terrama2.administration.services.registration', ['terrama2.services', 'terrama2.components.messagebox'])

  .controller('RegistrationController', ['$scope', '$window', 'ServiceInstanceFactory', 'Socket',
  function($scope, $window, ServiceInstanceFactory, Socket) {
    var socket = Socket;

    $scope.isCheckingConnection = false;
    $scope.services = [];

    $scope.service = configuration.service || {sshPort: 22};
    if ($scope.service.service_type_id) {
      $scope.service.service_type_id = $scope.service.service_type_id.toString();
    }

    // Defining default threads number
    if (!$scope.service.numberOfThreads) {
      $scope.service.numberOfThreads = 0;
    }

    // setting log instance
    $scope.log = $scope.service.log || {};

    $scope.update = false;
    if ($scope.service.name)
      $scope.update = true;

    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.isChecking = false;
    $scope.resetState = function() {
      $scope.display = false;
      $scope.alertBox.message = "";
    };

    // Getting all service instance to suggest database names
    ServiceInstanceFactory.get().success(function(services) {
      $scope.services = services;

      if ($scope.update)
        return;

      // process
      var ports = [];
      services.forEach(function(service) {
        ports.push(service.port);
      });

      socket.emit('suggestPortNumber', {ports: ports, host: $scope.service.host})
    }).error(function(err) {
      console.log(err);
    });

    // adding service port check listener
    socket.on('testPortNumberResponse', function(response) {
      console.log(response);
      if (response.error) {
        console.log("ERROR");
      } else {
        console.log("OK");
        $scope.service.port = response.port;
      }
    })

    // watching service type changed
    $scope.$watch('service.service_type_id', function(value) {
      switch(value) {
        case "1": // collect
        case "2": // analysis
          $scope.service.pathToBinary = $scope.service.pathToBinary || "terrama2_service";
          break;
        default: // none
          $scope.service.pathToBinary = $scope.service.pathToBinary;
          break;
      }
    });

    $scope.checkConnection = function() {
      if (!socket) {
        // TODO: error message
        return;
      }

      $scope.isCheckingConnection = true;
      $scope.ssh = {
        isLoading: true
      }

      $scope.db = {
        isLoading: true
      }

      setTimeout(function() {
        // SSH
        socket.emit('testSSHConnectionRequest',
          {
            host: $scope.service.host,
            port: $scope.service.sshPort,
            username: $scope.service.sshUser
          }
        )
        socket.on('testSSHConnectionResponse', function(result) {
          $scope.ssh.isLoading = false;
          if (result.error) {
            $scope.modalType = "modal-danger";
            $scope.ssh.isValid = false;
            $scope.ssh.message = result.message;
          } else {
            $scope.modalType = "modal-success";
            $scope.ssh.isValid = true;
          }
        });

        socket.emit('testDbConnection', $scope.log);
        socket.on('testDbConnectionResponse', function(result) {
          $scope.db.isLoading = false;
          if (result.error) {
            $scope.modalType = "modal-danger";
            $scope.db.isValid = false;
            $scope.db.message = result.message;
          } else {
            $scope.modalType = "modal-success";
            $scope.db.isValid = true;
            $scope.db.message = "";
          }
        })
      }, 1000);
    };

    $scope.save = function() {
      if ($scope.serviceForm.$invalid || $scope.logForm.$invalid) {
        angular.forEach($scope.serviceForm.$error, function (field) {
          angular.forEach(field, function(errorField){
            errorField.$setDirty();
          })
        });

        angular.forEach($scope.logForm.$error, function (field) {
          angular.forEach(field, function(errorField){
            errorField.$setDirty();
          })
        });
        return;
      }

      // validating enviroment
      if ($scope.service.runEnviroment)
        $scope.service.runEnviroment = $scope.service.runEnviroment.split("\n").join(" ");

      $scope.isChecking = true;
      $scope.display = false;
      $scope.alertBox.title = "Service Registration";

      ServiceInstanceFactory.post({
        service: $scope.service,
        log: $scope.log
      }).success(function(data) {
        $window.location.href = "/administration/services?token=" + data.token;
      }).error(function(err) {
        console.log(err);
        $scope.display = true;
        $scope.alertLevel = "alert-danger";
        $scope.alertBox.message = err.message;
      }).finally(function() {
        $scope.isChecking = false;
      })
    }
  }])
