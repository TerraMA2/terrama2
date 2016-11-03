(function() {
  "use strict";

  angular.module("terrama2.views.controllers.viewlist", [
    "terrama2",
    "terrama2.services",
    "terrama2.views.services",
    "terrama2.table",
    "terrama2.components.messagebox.services",
    "terrama2.components.messagebox"])
  .controller("ViewList", ViewList);

  /**
   * It handles TerraMA² View Listing.
   * @class ViewList
   */
  function ViewList($scope, i18n, ViewService, $log, MessageBoxService, $window, $q, Socket) {
    var self = this;

    // getting config from template
    var config = $window.configuration || {};

    /**
     * TerraMA² ViewService DAO
     * @type {ViewService}
     */
    self.ViewService = ViewService;

    /**
     * MessageBox object to handle message dialogs
     * @type {MessageBoxService}
     */
    self.MessageBoxService = MessageBoxService;

    // token message
    if (config.message) {
      self.MessageBoxService.success(i18n.__("View"), config.message);
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
     * It handles socket error.
     * 
     * @param {Object} response - Response object with error message value
     * @param {Object} response.message - Error message
     */
    Socket.on('errorResponse', function(response){
      self.MessageBoxService.danger(i18n.__("View"), response.message);
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

    // Initializing async modules
    $q.all([ViewService.init()])
      .then(function() {
        self.model = ViewService.list();

        /**
         * A URL to insert a new view
         * @type {string}
         */
        self.linkToAdd = "/configuration/views/new";

        /**
         * It makes a link to View edit
         * 
         * @param {View} object - Selected view
         * @returns {string}
         */
        self.link = function(object) {
          return "/configuration/views/edit/" + object.id;
        };

        /**
         * Icon properties to display in table like size, type (img/icon)
         * @type {Object}
         */
        self.iconProperties = {
          type: "img",
          width: 64,
          height: 64
        };

        /**
         * Icon functor to make URL pointing to icon.
         * 
         * @todo It should retrive a overview of layer
         * @returns {string}
         */
        self.icon = function(object) {
          return "/images/map-display.png";
        };

        /**
         * Functor to make URL to remove selected view
         * @param {Object}
         */
        self.remove = function(object) {
          return "/api/View/" + object.id + "/delete";
        };

        /**
         * Defines a properties to TerraMA² Table handle.
         * 
         * @type {Object}
         */
        self.extra = {
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
            var process_ids = {
              "ids":[object.id],
              "service_instance": object.service_instance_id
            };
            Socket.emit('run', process_ids);        
          }
        };
      })
      .catch(function(err) {
        $log.log("Could not load Views due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  ViewList.$inject = ["$scope", "i18n", "ViewService", "$log", "MessageBoxService", "$window", "$q", "Socket"];
} ());