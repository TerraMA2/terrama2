angular.module('terrama2.administration.services.registration',
  ['terrama2.services',
  'terrama2.components.messagebox']
).controller('RegistrationController', ['$scope', '$window', 'ServiceInstanceFactory', 'Socket', 'i18n',
  function($scope, $window, ServiceInstanceFactory, Socket, i18n) {
    var socket = Socket;

    // setting defaults
    $scope.css = {'boxType': 'box-solid'};

    $scope.initService = function(state) {
      $scope.service = $scope.service || {};

      if ($scope.update && $scope.service.host && $scope.service.host !== "")
        $scope.service.isLocal = false;
      else
        $scope.service.isLocal = state;
    };

    $scope.isCheckingConnection = false;
    $scope.services = [];
    $scope.i18n = i18n;

    $scope.service = angular.equals({}, configuration.service) ? {sshPort: 22} : configuration.service;
    if ($scope.service.service_type_id) {
      $scope.service.service_type_id = $scope.service.service_type_id.toString();
    }

    // Defining default threads number
    if (!$scope.service.numberOfThreads) {
      $scope.service.numberOfThreads = 0;
    }

    // setting log instance
    $scope.log = $scope.service.log || {};

    $scope.update = $scope.service.name ? true : false;

    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.isChecking = false;
    $scope.resetState = function() {
      $scope.display = false;
      $scope.alertBox.message = "";
    };

    $scope.extraProperties = {};
    $scope.config = {
      availableDatabases: []
    };

    // Getting all service instance to suggest database names
    ServiceInstanceFactory.get().success(function(services) {
      $scope.services = services;

      if ($scope.update)
        return;

      // process
      var ports = [];
      services.forEach(function(service) {
        $scope.config.availableDatabases.push(Object.assign({name: service.name}, service.log));
        ports.push(service.port);
      });

      socket.emit('suggestPortNumber', {ports: ports, host: $scope.service.host})
    }).error(function(err) {
      console.log(err);
    });

    // adding service port check listener
    socket.on('suggestPortNumberResponse', function(response) {
      console.log(response);
      if (response.error) {
        console.log("ERROR");
      } else {
        console.log("OK");
        $scope.service.port = response.port;
      }
    });

    // watching service type changed
    $scope.$watch('service.service_type_id', function(value) {
      switch(value) {
        case "1": // collect
        case "2": // analysis
          $scope.service.pathToBinary = $scope.service.pathToBinary || "terrama2_service";
          break;
        default: // none
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
      };

      $scope.db = {
        isLoading: true
      };

      setTimeout(function() {
        // SSH
        socket.emit('testSSHConnectionRequest',
          {
            host: $scope.service.host,
            port: $scope.service.sshPort,
            username: $scope.service.sshUser,
            isLocal: $scope.service.isLocal,
            pathToBinary: $scope.service.pathToBinary
          }
        );
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

    $scope._save = function() {
      $scope.isChecking = true;
      $scope.display = false;
      $scope.alertBox.title = "Service Registration";
      var request;

      if ($scope.update) {
        request = ServiceInstanceFactory.put($scope.service.id, {
          service: $scope.service,
          log: $scope.log
        });
      }
      else {
        request = ServiceInstanceFactory.post({
          service: $scope.service,
          log: $scope.log
        });
      }

      request.success(function(data) {
        $window.location.href = "/administration/services?token=" + data.token+"&service="+data.service + "&restart="+data.restart;
      }).error(function(err) {
        console.log(err);
        $scope.display = true;
        $scope.alertLevel = "alert-danger";
        $scope.alertBox.message = err.message;
      }).finally(function() {
        $scope.isChecking = false;
      })
    };

    socket.on('testPortNumberResponse', function(response) {
      if (response.error) {
        if (configuration.service.port !== $scope.service.port) {
          $scope.alertLevel = "alert-danger";
          $scope.alertBox.message = response.message;
          $scope.display = true;
          return;
        }
      }

      // continue save process

      // checking port number with a registered service
      for(var i = 0; i < $scope.services.length; ++i) {
        var service = $scope.services[i];

        if (service.port === $scope.service.port) {
          $scope.alertLevel = "alert-warning";
          $scope.alertBox.title = i18n.__("Service Registration");
          $scope.alertBox.message = i18n.__("There is already a service registered in same port ") + service.port + " (" + service.name + ")";
          $scope.display = true;
          $scope.extraProperties.confirmButtonFn = $scope._save;
          $scope.extraProperties.object = {};
          return;
        }
      }

      $scope._save();
    });

    $scope.save = function() {
      $scope.$broadcast('formFieldValidation');

      if ($scope.serviceForm.$invalid || $scope.logForm.$invalid) {
        return;
      }

      // validating enviroment
      if ($scope.service.runEnviroment)
        $scope.service.runEnviroment = $scope.service.runEnviroment.split("\n").join(" ");

      if ($scope.service.isLocal) {
        $scope.service.host = "";
      }

      // testing port number
      socket.emit('testPortNumber', {port: $scope.service.port});
    }
  }]);
