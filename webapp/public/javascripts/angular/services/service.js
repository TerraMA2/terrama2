(function() {
  'use strict';

  angular.module('terrama2.administration.services', [
    'terrama2.administration.services.iservices',
    'terrama2.table',
    'terrama2.components.messagebox',
    'terrama2.components.messagebox.services'
  ])
  .controller('ListController', ['Service', 'MessageBoxService', 'Socket', 'i18n', '$log', '$locale',
    function(Service, MessageBoxService, Socket, i18n, $log, $locale) {
      var self = this;

      self.service = Service;
      self.messageBoxService = MessageBoxService;

      Service.init().then(function() {
        var model = self.service.list();

        if (model.length === 0) {
          self.extra.service.starting = false;
          return;
        }

        model.forEach(function(service) {
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

        self.service.model = model;

        var config = configuration;

        model.forEach(function(service) {
          if (config.message && parseInt(config.service) === service.id && config.restart) {
            Socket.emit('start', {service: service.id});
          } else {
            Socket.emit('status', {service: service.id});
          }
        });

        self.title = i18n.__('Services');
        self.helperMessage = "This page shows available services in TerraMA2 application";

        // terrama2 box
        self.boxCss = {};

        self.link = function(object) {
            return "/administration/services/" + object.id;
        };

        self.remove = function(object) {
          return "/api/Service/" + object.id + "/delete";
        };

        self.confirmRemoval = function(object) {
          self.target = object;
          $("#removalID").modal();
        };

        // listeners
        Socket.on('statusResponse', function(response) {
          var service = self.service.get(response.service);

          if (!service) {
            return;
          }

          if (response.hasOwnProperty('loading')) {
            service.loading = response.loading;
          }

          service.online = response.online;
        });

        Socket.on('stopResponse', function(response) {
          var service = self.service.get(response.service);

          if (!service) {
            return;
          }

          service.loading = response.loading;
          service.online = response.online;

          if (!response.loading) {
            service.requestingForClose = false;
          }
        });

        Socket.on('closeResponse', function(response) {
          var service = self.service.get(response.service);

          if (!service) {
            return;
          }

          service.loading = false;
          service.online = false;
          service.requestingForClose = false;
        });

        Socket.on('errorResponse', function(response) {
          var service = self.service.get(response.service);

          if (!service) {
            return;
          }

          service.loading = false;
          service.online = response.online;
        });

        if (config.message) {
          self.messageBoxService.success(i18n.__('Service'), i18n.__(config.message));
        }

        self.fields = [
          {key: 'name', as: 'Name'},
          {key: 'type', as: 'Type'}
        ];

        self.iconFn = null;

        self.linkToAdd = "/administration/services/new";

        self.extra = {
          removeOperationCallback: function(err, data) {
            if (err) {
              self.messageBoxService.error(i18n.__('Service'), err.message);
              return;
            }
            self.messageBoxService.success(i18n.__('Service'), data.name + " removed");
          },

          service: {
            starting: false,
            stoping: false,
            startAll: function() {
              self.extra.service.starting = true;
              var model = self.service.list();
              model.forEach(function(modelInstance) {
                if (!modelInstance.online) {
                  if (!modelInstance.loading) {
                    modelInstance.loading = true;
                    Socket.emit('start', {service: modelInstance.id});
                  }
                }
              });

              Socket.once('statusResponse', function(response) {
                self.extra.service.starting = false;
              });
            },

            stopAll: function() {
              self.extra.service.stoping = true;
              self.service.list().forEach(function(modelInstance) {
                if (modelInstance.online) {
                  if (!modelInstance.loading) {
                    Socket.emit('stop', {service: modelInstance.id});
                  }
                }
              });

              Socket.once('closeResponse', function(response) {
                self.extra.service.stoping = false;
              });
            },

            hasServiceOffline: function() {
              if (self.service.list().length === 0) {
                return false;
              }
              return self.service.list().some(function(instance) {
                if (!instance.online) {
                  return true;
                }
              });
            },

            handler: function(serviceInstance) {
              if (!serviceInstance.online) {
                Socket.emit('start', {service: serviceInstance.id});
              } else {
                serviceInstance.requestingForClose = true;
                Socket.emit('stop', {service: serviceInstance.id});
              }
            }
          }
        }
      }).catch(function(err) {
        $log.error("Could not initialize services. Please refresh page (F5). " + err.toString());
      });   
  }]);
}
());