define([], function(){
  "use strict";

  function AlertList($scope, i18n, $q, AlertService, MessageBoxService){
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

    $q.all([AlertService.init()])
      .then(function(){
        self.model = AlertService.list();

        self.linkToAdd = "/configuration/alerts/new";

        /**
         * It makes a link to Alert edit
         *
         * @param {View} object - Selected view
         * @returns {string}
         */
        self.link = function(object) {
          return "";
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

         self.icon = function(object) {
           return "/images/alert/monitored-object/monitored-object_alert.png"
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
              MessageBoxService.danger(i18n.__("View"), err.message);
              return;
            }
            MessageBoxService.success(i18n.__("View"), data.result.name + i18n.__(" removed"));
          }
        }
        /**
         * Functor to make URL to remove selected view
         * @param {Object}
         */
        self.remove = function(object) {
          return "/api/Alert/" + object.id + "/delete";
        };
      })
      .catch(function(err) {
        $log.log("Could not load alerts due " + err.toString() + ". Please refresh page (F5)");
      });
  }

  AlertList.$inject = ["$scope", "i18n", "$q", "AlertService", "MessageBoxService"];

  return AlertList;
});