define([], function(){
  "use strict";

  function AlertList($scope, i18n, $q, AlertService, LegendService, MessageBoxService, Socket, Service, $window, $log, $timeout) {
    var config = $window.configuration;
    var globals = $window.globals;

    var self = this;
    self.i18n = i18n;

    self.AlertService = AlertService;
    self.LegendService = LegendService;

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
     * List of legends
     * @type {Object[]}
     */
    self.legendModel = [];

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
      var errorMessage = response.message;
      var targetMethod = null;
      if (serviceCache[response.service] != undefined){
        var service = Service.get(serviceCache[response.service].process_ids.service_instance);
        if (service != null) {
          errorMessage = i18n.__(errorMessage) + " " + i18n.__("Service") + ": '" + service.name + "' ";
        }
        if (config.extra && config.extra.id){
          var warningMessage = config.message + ". ";
          errorMessage = warningMessage + errorMessage;
          targetMethod = MessageBoxService.warning; // setting warning method to display message
          delete config.extra;
          delete config.message;
        } 
        else {
          targetMethod = MessageBoxService.danger;
        }
      }

      if(targetMethod && targetMethod.call)
        targetMethod.call(MessageBoxService, i18n.__("Alerts"), errorMessage);

      delete serviceCache[response.service];
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
          if(serviceCache[response.service]) {
            if(serviceCache[response.service].view && serviceCache[response.service].view.id)
              self.extra.run(serviceCache[response.service].view);
            else
              Socket.emit('run', serviceCache[response.service].process_ids);

            delete $scope.disabledButtons[serviceCache[response.service].service_id];
            delete serviceCache[response.service];
          }

          self.servicesInstances[response.service] = true;
        } else {
          if(serviceCache[response.service])
            delete $scope.disabledButtons[serviceCache[response.service].service_id];

          self.servicesInstances[response.service] = false;
        }
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

    /**
     * Legend fields to display in table
     * @type {Object[]}
     */
    self.legendFields = [{
      key: "name",
      as: i18n.__("Name")
    }, {
      key: "description",
      as: i18n.__("Description")
    }];

    self.servicesInstances = {};

    $q.all(
      [
        AlertService.init(),
        LegendService.init()
      ]
    )
      .then(function(){
        self.model = AlertService.list();
        self.legendModel = LegendService.list();

        self.model.forEach(function(instance) {
          Socket.emit("status", { service: instance.service_instance_id });
          self.servicesInstances[instance.service_instance_id] = false;
        });

        self.linkToAdd = BASE_URL + "configuration/alerts/new";
        self.legendLinkToAdd = BASE_URL + "configuration/legends/new";

        self.activeTab = (config.legendsTab ? 2 : 1);

        if(config.message !== "") {
          var messageArray = config.message.split(" ");
          var tokenCodeMessage = messageArray[messageArray.length - 1];
          messageArray.splice(messageArray.length - 1, 1);

          $timeout(function() {
            var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
            if(!self.MessageBoxService.alertBox.display) self.MessageBoxService.success((config.legendsTab ? i18n.__("Legends") : i18n.__("Alerts")), finalMessage);
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

        self.statusChangeLink = function(object) {
          return BASE_URL + "configuration/alerts/changeStatus/" + object.id;
        };

        /**
         * It makes a link to Legend edit
         *
         * @param {Legend} object - Selected legend
         * @returns {string}
         */
        self.legendLink = function(object) {
          return BASE_URL + "configuration/legends/edit/" + object.id;
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
           if (object.dataSeries.isAnalysis && object.dataSeries.semantics == "ANALYSIS_MONITORED_OBJECT-postgis"){
            return BASE_URL + "images/alert/monitored-object/monitored-object_alert.png";
           } else {
            return BASE_URL + "images/alert/grid/grid_alert.png";
           }
         }

        /**
         * Defines a properties to TerraMA² Table handle.
         *
         * @type {Object}
         */
        self.extra = {
          canRemove: config.hasProjectPermission,
          advancedFilters: [
            {
              name: "Analysis",
              value: "Analysis",
              checked: true
            },
            {
              name: "Dynamic Data",
              value: "Dynamic Data",
              checked: true
            }
          ],
          executeAdvancedFilter: function() {
            for(var i = 0, advancedFiltersLength = self.extra.advancedFilters.length; i < advancedFiltersLength; i++) {
              for(var j = 0, modelLength = self.model.length; j < modelLength; j++) {
                var semantics = self.model[j].dataSeries.semantics;
                var type;

                switch(semantics) {
                  case "ANALYSIS_MONITORED_OBJECT-postgis":
                    type = i18n.__("Analysis");
                    break;

                  case "DCP-toa5":
                  case "DCP-generic":
                  case "DCP-inpe":
                  case "DCP-postgis":
                  case "Occurrence-generic":
                  case "OCCURRENCE-wfp":
                  case "OCCURRENCE-lightning":
                  case "OCCURRENCE-postgis":
                    type = i18n.__("Dynamic Data");
                    break;

                  case "GRID-gdal":
                  case "GRID-geotiff":
                  case "GRID-ascii":
                  case "GRID-grads":
                  case "GRID-grib":
                    if(self.model[j].dataSeries.isAnalysis)
                      type = i18n.__("Analysis");
                    else
                      type = i18n.__("Dynamic Data");
                    break;

                  default:
                    type = i18n.__("Dynamic Data");
                    break;
                }

                if(i18n.__(self.extra.advancedFilters[i].value) === type) {
                  if(self.extra.advancedFilters[i].checked)
                    self.model[j].showInTable = true;
                  else
                    self.model[j].showInTable = false;
                }
              }
            }
          },
          removeOperationCallback: function(err, data) {
            MessageBoxService.reset();
            if (err) {
              if (err.serviceStoppedError){
                var errorWhenDeleteMessage = i18n.__("Can not delete the alert if the service is not running. ");
                if(err.service && err.service.instance_name)
                  errorWhenDeleteMessage += i18n.__("Service") + ": " + err.service.instance_name;
                return MessageBoxService.danger(i18n.__("Alerts"), errorWhenDeleteMessage);            
              } else {
                MessageBoxService.danger(i18n.__("Alerts"), i18n.__(err.message));
                return;
              }
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
              "service_name": object.name,
              "view": object.view
            };

            $scope.disabledButtons[object.id] = true;

            Socket.emit('status', {service: object.service_instance_id});
          },
          disabledButtons: function(object){
            return $scope.disabledButtons[object.id];
          }

        }

        /**
         * Defines a properties to TerraMA² Table handle.
         *
         * @type {Object}
         */
        self.legendExtra = {
          canRemove: config.hasProjectPermission,
          removeOperationCallback: function(err, data) {
            if(err && err.message) {
              MessageBoxService.reset();
              MessageBoxService.danger(i18n.__("Legends"), i18n.__(err.message));
              return;
            } else if (data){
              MessageBoxService.reset();
              MessageBoxService.success(i18n.__("Legends"), data.result.name + i18n.__(" removed"));
              return;
            }
          },
          disabledButtons: function(object) {
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

        /**
         * Function to make URL to remove selected legend
         * @param {Object}
         */
        self.removeLegend = function(object) {
          return BASE_URL + "api/Legend/" + object.id + "/delete";
        };
      })
      .catch(function(err) {
        $log.log("Could not load alerts due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  AlertList.$inject = ["$scope", "i18n", "$q", "AlertService", "LegendService", "MessageBoxService", "Socket", "Service", "$window", "$log", "$timeout"];

  return AlertList;
});