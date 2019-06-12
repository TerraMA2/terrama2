define([], function() {
  "use strict";

  /**
   * It handles TerraMA² View Listing.
   * @class ViewList
   *
   * @param {angular.IScope} $scope - Angular scope
   * @param {Object} i18n - TerraMA² i18n module
   * @param {angular.ILog} $log - Angular log module
   * @param {MessageBoxService} MessageBoxService - TerraMA² Message dialog object
   * @param {angluar.IWindow} $window - Angular window module
   * @param {angular.IQ} $q - Angular promiser module
   * @param {Object} Socket - TerraMA² Socket io module
   * @param {Service} Service - TerraMA² Service object
   */
  function ViewList($scope, i18n, ViewService, $log, MessageBoxService, $window, $q, Socket, Service, $timeout, DataSeriesSemanticsService) {
    /**
     * View List controller
     * @type {ViewList}
     */
    var self = this;

    // getting config from template
    var config = $window.configuration || {};

    /**
     * TerraMA² ViewService DAO
     * @type {ViewService}
     */
    self.ViewService = ViewService;

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

    // token message
    if(config.message) {
      var messageArray = config.message.split(" ");
      var tokenCodeMessage = messageArray[messageArray.length - 1];
      messageArray.splice(messageArray.length - 1, 1);

      $timeout(function() {
        var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
        if(!self.MessageBoxService.alertBox.display) self.MessageBoxService.success(i18n.__("View"), finalMessage);
      }, 1000);
    }

    /**
     * It represents a cached views
     * @type {Object[]}
     */
    self.model = [];

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
    Socket.on('errorResponse', function(response) {
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
        targetMethod.call(MessageBoxService, i18n.__("View"), errorMessage);

      delete serviceCache[response.service];
    });

    /**
     * It handles process run feedback. It just show message box with success message
     *
     * @param {Object} response - Response object
     * @param {Object} response.service - TerraMA² service id to determines which service called
     */
    Socket.on('runResponse', function(response){
      self.MessageBoxService.success(i18n.__("View"), i18n.__("The process was started successfully"));
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

    self.servicesInstances = {};

    // Initializing async modules
    $q.all([ViewService.init(), DataSeriesSemanticsService.init()])
      .then(function() {
        //Dont show views created by alerts
        var viewRestriction = {
          source_type: '!4'
        }
        // Setting loaded views into model
        self.model = ViewService.list(viewRestriction);

        self.model.forEach(function(instance) {
          Socket.emit("status", { service: instance.service_instance_id });
          self.servicesInstances[instance.service_instance_id] = false;
        });

        /**
         * A URL to insert a new view
         * @type {string}
         */
        self.linkToAdd = BASE_URL + "configuration/views/new";

        /**
         * It makes a link to View edit
         *
         * @param {View} object - Selected view
         * @returns {string}
         */
        self.link = function(object) {
          return BASE_URL + "configuration/views/edit/" + object.id;
        };

        self.statusChangeLink = function(object) {
          return BASE_URL + "api/View/changeStatus/" + object.id;
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

        /**
         * Icon functor to make URL pointing to icon.
         *
         * @todo It should retrive a overview of layer
         * @returns {string}
         */
        self.icon = function(object) {
          const semantics = DataSeriesSemanticsService.get(object.dataSeries.semantics);
          switch (semantics.data_series_type_name){
            case "OCCURRENCE":
              return BASE_URL + "images/view/dynamic_data_series/occurrence/large_occurrence_view.png";
              break;
            case "GEOMETRIC_OBJECT":
            case "STATIC_DATA":
              return BASE_URL + "images/view/static_data_series/vetor/large_vector_view.png";
              break;
            case "GRID":
              return BASE_URL + "images/view/static_data_series/grid/large_grid_view.png";
              break;
            case "DCP":
              return BASE_URL + "images/view/dynamic_data_series/dcp/large_dcp_view.png";
              break;
            case "ANALYSIS_MONITORED_OBJECT":
              return BASE_URL + "images/view/analysis/large_analysis_view.png";
              break;
            default:
              return BASE_URL + "images/view/dynamic_data_series/grid/large_grid_view.png";
              break;
          }
        };

        /**
         * Functor to make URL to remove selected view
         * @param {Object}
         */
        self.remove = function(object) {
          return BASE_URL + "api/View/" + object.id + "/delete";
        };

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
              value: 3,
              checked: true
            },
            {
              name: "Dynamic Data",
              value: 2,
              checked: true
            },
            {
              name: "Static Data",
              value: 1,
              checked: true
            }
          ],
          advancedFilterField: "source_type",
          removeOperationCallback: function(err, data) {
            MessageBoxService.reset();
            if (err) {
              if (err.serviceStoppedError){
                var errorWhenDeleteMessage = i18n.__("Can not delete the view if the service is not running. ");
                if(err.service && err.service.instance_name)
                  errorWhenDeleteMessage += i18n.__("Service") + ": " + err.service.instance_name;
                return MessageBoxService.danger(i18n.__("View"), errorWhenDeleteMessage);            
              } else {
                MessageBoxService.danger(i18n.__("View"), i18n.__(err.message));
                return;
              }
            }
            MessageBoxService.success(i18n.__("View"), data.result.name + i18n.__(" removed"));
          },
          showRunButton: true,
          canRun: function(object) {
            return object;
          },
          /**
           * It defines a process run button, in order to run now
           *
           * @param {View} object - Selected view
           */
          run: function(object){
            serviceCache[object.service_instance_id] = {
              "process_ids": {
                "ids":[object.id],
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
        };

        if(config.extra && config.extra.id) {
          var viewToRun = self.model.filter(function(element) {
            return element.id == config.extra.id;
          });
          if(viewToRun.length == 1) {
            self.extra.run(viewToRun[0]);
          }
        }
      })
      .catch(function(err) {
        $log.log("Could not load Views due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  ViewList.$inject = ["$scope", "i18n", "ViewService", "$log", "MessageBoxService", "$window", "$q", "Socket", "Service", "$timeout", "DataSeriesSemanticsService"];

  return ViewList;
});
