define([], function() {
  'use strict';

  function ListController($scope, Service, $HttpTimeout, Socket, i18n, MessageBoxService, $timeout) {
    $scope.socket = Socket;

    $scope.title = i18n.__('Services');
    $scope.MessageBoxService = MessageBoxService;
    $scope.helperMessage = "This page shows available services in TerraMA2 application";

    // terrama2 box
    $scope.boxCss = {};

    $scope.link = function(object) {
      return "/administration/services/" + object.id;
    };

    $scope.model = [];

    $scope.remove = function(object) {
      return "/api/Service/" + object.id + "/delete";
    };

    $scope.confirmRemoval = function(object) {
      $scope.target = object;
      $("#removalID").modal();
    };

    var getModel = function(serviceId) {
      var output = null;
      $scope.model.some(function(instance) {
        if (instance.id === serviceId) {
          output = instance;
          return true;
        }
      });
      return output;
    };

    // listeners
    $scope.socket.on('statusResponse', function(response) {
      var service = getModel(response.service);

      service.checking = response.checking;

      if (!service) {
        return;
      }

      if (response.hasOwnProperty('loading')) {
        service.loading = response.loading;
      }

      if (response.shutting_down) {
        service.loading = response.shutting_down;
      }

      service.online = response.online;
    });

    $scope.socket.on("serviceVersion", function(response) {
      if (!response.match) {
        MessageBoxService.warn($scope.title, i18n.__("It seems you are using a different versions of TerraMA². Current version of TerraMA² Web is " + response.current + " " +i18n.__("but the TerraMA² service version is") + " " + response.response + ". " +i18n.__("Some operations may not work properly")));
      }
    });

    $scope.socket.on('stopResponse', function(response) {
      var service = getModel(response.service);

      if (!service)
        return;

      service.loading = response.loading;
      service.online = response.online;

      if (!response.loading)
        service.requestingForClose = false;
    });

    $scope.socket.on('closeResponse', function(response) {
      var service = getModel(response.service);

      if (!service)
        return;

      service.loading = false;
      service.online = false;
      service.requestingForClose = false;
      service.stoping = false;
    });

    $scope.socket.on('errorResponse', function(response) {
      var service = getModel(response.service);

      if (!service)
        return;

      service.loading = false;
      service.online = response.online;
    })

    Service.init().then(function(services) {
      if (services.length === 0) {
        $scope.extra.service.starting = false;
        return;
      }

      services.forEach(function(service) {
        switch(service.service_type_id) {
          case 1:
            service.type = i18n.__("Collect");
            break;
          case 2:
            service.type = i18n.__("Analysis");
            break;
          case 3:
            service.type = i18n.__("View");
            break;
          default:
            break;
        }
      });

      $scope.model = services;

      services.forEach(function(service) {
        if (configuration.message && parseInt(configuration.service) === service.id && configuration.restart) {
          $scope.socket.emit('start', {service: service.id});
        } else {
          $scope.socket.emit('status', {service: service.id});
        }
      });
    }).catch(function(err) {
      console.log(err);
    });

    $scope.close = function() { MessageBoxService.reset(); };
    if (configuration.message) {
      MessageBoxService.success($scope.title, configuration.message);
    }

    $scope.fields = [
      {key: 'name', as: 'Name'},
      {key: 'type', as: 'Type'}];
    $scope.iconFn = null;

    $scope.linkToAdd = "/administration/services/new";

    $scope.extra = {
      removeOperationCallback: function(err, data) {
        if (err) {
          MessageBoxService.danger($scope.title, err.message);
          return;
        }
        MessageBoxService.success($scope.title, data.name + " removed");
      },

      service: {
        starting: false,
        stoping: false,
        startAll: function() {
          $scope.extra.service.starting = true;
          $scope.model.forEach(function(modelInstance) {
            if (!modelInstance.online) {
              if (!modelInstance.loading) {
                modelInstance.loading = true;
                $scope.socket.emit('start', {service: modelInstance.id});
              }
            }
          });
          // It enable again the button. It must be removed after once there is no service executing
          $timeout(function() {
            $scope.extra.service.starting = false;
          }, 3000);
        },

        stopAll: function() {
          $scope.extra.service.stoping = true;
          $scope.model.forEach(function(modelInstance) {
            if (modelInstance.online) {
              if (!modelInstance.loading) {
                $scope.socket.emit('stop', {service: modelInstance.id});
              }
            }
          });

          $scope.extra.service.stoping = false;
          $scope.extra.service.starting = false;
        },

        hasServiceOffline: function() {
          if ($scope.model.length === 0) {
            return false;
          }
          return $scope.model.some(function(instance) {
            if (!instance.online) {
              return true;
            }
          });
        },

        handler: function(serviceInstance) {
          if (!serviceInstance.online) {
            $scope.socket.emit('start', {service: serviceInstance.id});
          } else {
            serviceInstance.requestingForClose = true;
            $scope.socket.emit('stop', {service: serviceInstance.id});
          }
        },
        reload: function(serviceInstance) {
          $HttpTimeout({
            url: "/api/Remote/reload",
            data: {serviceId: serviceInstance.id},
            method: 'post'
          }).then(function(data) {
            serviceInstance.online = data.online;
            console.log(data);
          }).catch(function(err) {
            console.log("Error in ping: ", err);
          })
        }
      }
    };
  }

  ListController.$inject = ['$scope', 'Service', '$HttpTimeout', 'Socket', 'i18n', 'MessageBoxService', '$timeout'];

  return ListController;
});