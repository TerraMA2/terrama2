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
  function ViewList($scope, i18n, ViewService, $log, MessageBoxService, $window, $q, Socket, Service, $timeout) {
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
        self.MessageBoxService.success(i18n.__("View"), finalMessage);
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
          Socket.emit('run', serviceCache[response.service].process_ids);
          delete $scope.disabledButtons[serviceCache[response.service].service_id];
          delete serviceCache[response.service];
        } 
        else {
          delete $scope.disabledButtons[serviceCache[response.service].service_id];
        }
      }
    });

    // Initializing async modules
    $q.all([ViewService.init()])
      .then(function() {
        //Dont show views created by alerts
        var viewRestriction = {
          source_type: '!4'
        }
        // Setting loaded views into model
        self.model = ViewService.list(viewRestriction);

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
          switch(object.dataSeries.semantics){
            case "OCCURRENCE-postgis":
            case "OCCURRENCE-wfp":
            case "OCCURRENCE-lightning":
            case "Occurrence-generic":
              return BASE_URL + "images/view/dynamic_data_series/occurrence/large_occurrence_view.png";
              break;
            case "STATIC_DATA-postgis":
            case "STATIC_DATA-ogr":
              return BASE_URL + "images/view/static_data_series/vetor/large_vector_view.png";
              break;
            case "GRID-static_gdal":
              return BASE_URL + "images/view/static_data_series/grid/large_grid_view.png";
              break;
            case "DCP-inpe":
            case "DCP-toa5":
            case "DCP-postgis":
            case "DCP-generic":
              return BASE_URL + "images/view/dynamic_data_series/dcp/large_dcp_view.png";
              break;
            case "ANALYSIS_MONITORED_OBJECT-postgis":
              return BASE_URL + "images/view/analysis/large_analysis_view.png";
              break;
            case "GRID-gdal":
            case "GRID-grads":
              return BASE_URL + "images/view/dynamic_data_series/grid/large_grid_view.png";
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
            },
            {
              name: "Static Data",
              value: "Static Data",
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

                  case "STATIC_DATA-postgis":
                  case "STATIC_DATA-ogr":
                  case "GRID-static_gdal":
                    type = i18n.__("Static Data");
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
              MessageBoxService.danger(i18n.__("View"), i18n.__(err.message));
              return;
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

  ViewList.$inject = ["$scope", "i18n", "ViewService", "$log", "MessageBoxService", "$window", "$q", "Socket", "Service", "$timeout"];

  return ViewList;
});
