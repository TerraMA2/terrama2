angular.module('terrama2.administration.services', ['terrama2.table', 'terrama2.services', 'terrama2.components.messagebox'])
  .controller('ListController', ['$scope', 'ServiceInstanceFactory', '$HttpTimeout', 'Socket', 'i18n',
    function($scope, ServiceInstanceFactory, $HttpTimeout, Socket, i18n) {
      $scope.socket = Socket;

      $scope.title = i18n.__('Services');
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

      var getModel = function(serviceId) {
        var output = null;
        $scope.model.some(function(instance) {
          if (instance.id === serviceId) {
            output = instance;
            return true;
          }
        })
        return output;
      }

      // listeners
      $scope.socket.on('statusResponse', function(response) {
        var service = getModel(response.service);

        if (!service)
          return;

        if (response.hasOwnProperty('loading'))
          service.loading = response.loading;

        service.online = response.online;
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
      })

      $scope.socket.on('errorResponse', function(response) {
        var service = getModel(response.service);

        if (!service)
          return;

        service.loading = false;
        service.online = response.online;
      })

      ServiceInstanceFactory.get().success(function(services) {
        services.forEach(function(service) {
          switch(service.service_type_id) {
            case 1:
              service.type = "Collect";
              break;
            case 2:
              service.type = "Analysis";
              break;
            default:
              break;
          }
        });

        $scope.model = services;

        services.forEach(function(service) {
          if (configuration.message && parseInt(configuration.service) === service.id && configuration.restart) {
            $scope.socket.emit('start', {service: service.id});
          } else
            $scope.socket.emit('status', {service: service.id});
        });

        // todo: ping each one to check current state
      }).error(function(err) {
        console.log(err);
      });

      $scope.resetState = function() { $scope.display = false; };
      $scope.alertLevel = "alert-success";
      $scope.alertBox = {
        title: "Service",
        message: configuration.message
      };
      $scope.display = configuration.message !== "";

      $scope.fields = [
        {key: 'name', as: 'Name'},
        {key: 'type', as: 'Type'}];
      $scope.iconFn = null;

      $scope.linkToAdd = "/administration/services/new";

      $scope.extra = {
        removeOperationCallback: function(err, data) {
          $scope.display = true;
          if (err) {
            $scope.alertLevel = "alert-danger";
            $scope.alertBox.message = err.message;
            return;
          }

          $scope.alertLevel = "alert-success";
          $scope.alertBox.message = data.name + " removed";
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

            $scope.socket.once('statusResponse', function(response) {
              $scope.extra.service.starting = false;
            });
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

            $scope.socket.once('closeResponse', function(response) {
              $scope.extra.service.stoping = false;
            });
          },

          hasServiceOffline: function() {
            return $scope.model.some(function(instance) {
              if (!instance.online) {
                return true;
              }
            })
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
      }
  }])
