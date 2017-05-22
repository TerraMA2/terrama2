define([], function(){
  "use strict";

  function AlertList($scope, i18n, $q, AlertService, MessageBoxService, Socket, Service, $window, $log, $timeout) {
    var config = $window.configuration;
    var globals = $window.globals;

    var self = this;
    self.i18n = i18n;

    self.AlertService = AlertService;

    /**
     * Helper to reset alert box instance
     */
    self.close = function() {
      self.MessageBoxService.reset();
    };

    /**
     * MessageBox object to handle message dialogs (Singleton)
     * @type {MessageBoxService}
     */
    self.MessageBoxService = MessageBoxService;

    /**
     * It represents a cached alerts
     * @type {Object[]}
     */
    self.model = [];    
    /**
     * Control to disable the run buttons
     * @type {Object}
     */

    $scope.disabledButtons = {};
    /**
     * Keeps services data
     * @type {Object}
     */
    var serviceCache = {};

    /**
     * Starts the module 'Service'
     */
    Service.init();

    /**
     * It handles socket error.
     *
     * @param {Object} response - Response object with error message value
     * @param {Object} response.message - Error message
     */
    Socket.on('errorResponse', function(response){
      self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__(response.message));
    });

    /**
     * It handles process run feedback. It just show message box with success message
     *
     * @param {Object} response - Response object
     * @param {Object} response.service - TerraMA² service id to determines which service called
     */
    Socket.on('runResponse', function(response){
      self.MessageBoxService.success(i18n.__("Alerts"), i18n.__("The process was started successfully"));
    });

    /**
     * It handles process status feedback.
     *
     * @param {Object} response - Response object
     */
    Socket.on('statusResponse', function(response) {
      if(response.checking === undefined || (!response.checking && response.status == 400)) {
        if(response.online) {
          Socket.emit('run', serviceCache[response.service].process_ids);
        } else {
          if(serviceCache[response.service] != undefined) {
            var service = Service.get(serviceCache[response.service].process_ids.service_instance);

            if(service != null) {
              self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__("Service") + " '" + service.name + "' " + i18n.__("is not active"));
            } else {
              self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__("Service not active"));
            }
          } else {
            self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__("Service not active"));
          }
        }

        delete $scope.disabledButtons[serviceCache[response.service].service_id];
        delete serviceCache[response.service];
      }
    });

    /**
     * Fields to display in table
     * @type {Object[]}
     */
    self.fields = [{
      key: "name",
      as: i18n.__("Name")
    }, {
      key: "description",
      as: i18n.__("Description")
    }];

    $q.all([AlertService.init()])
      .then(function(){
        self.model = AlertService.list();

        self.linkToAdd = BASE_URL + "configuration/alerts/new";

        if(config.message !== "") {
          var messageArray = config.message.split(" ");
          var tokenCodeMessage = messageArray[messageArray.length - 1];
          messageArray.splice(messageArray.length - 1, 1);

          $timeout(function() {
            var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
            self.MessageBoxService.success(i18n.__("Alerts"), finalMessage);
          }, 1000);
        }

        /**
         * It makes a link to Alert edit
         *
         * @param {Alert} object - Selected alert
         * @returns {string}
         */
        self.link = function(object) {
          return BASE_URL + "configuration/alerts/edit/" + object.id;
          //return "";
        };

        /**
         * Icon properties to display in table like size, type (img/icon)
         * @type {Object}
         */
        self.iconProperties = {
          type: "img",
          width: 20,
          height: 20
        };

         self.icon = function(object) {
           return BASE_URL + "images/alert/monitored-object/monitored-object_alert.png"
         }

        /**
         * Defines a properties to TerraMA² Table handle.
         *
         * @type {Object}
         */
        self.extra = {
          removeOperationCallback: function(err, data) {
            MessageBoxService.reset();
            if (err) {
              MessageBoxService.danger(i18n.__("Alerts"), i18n.__(err.message));
              return;
            }
            MessageBoxService.success(i18n.__("Alerts"), data.result.name + i18n.__(" removed"));
          },
          showRunButton: true,
          canRun: function(object) {
            return object;
          },
          /**
           * It defines a process run button, in order to run now
           *
           * @param {Alert} object - Selected alert
           */
          run: function(object){
            serviceCache[object.service_instance_id] = {
              "process_ids": {
                "ids":[object.id],
                "execution_date": new Date().toISOString(),
                "service_instance": object.service_instance_id
              },
              "service_id": object.id,
              "service_name": object.name
            };

            $scope.disabledButtons[object.id] = true;

            Socket.emit('status', {service: object.service_instance_id});
          },
          disabledButtons: function(object){
            return $scope.disabledButtons[object.id];
          }

        }
        /**
         * Functor to make URL to remove selected alert
         * @param {Object}
         */
        self.remove = function(object) {
          return BASE_URL + "api/Alert/" + object.id + "/delete";
        };
      })
      .catch(function(err) {
        $log.log("Could not load alerts due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  AlertList.$inject = ["$scope", "i18n", "$q", "AlertService", "MessageBoxService", "Socket", "Service", "$window", "$log", "$timeout"];

  return AlertList;
});