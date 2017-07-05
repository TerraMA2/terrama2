define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle Legend form registration.
   * @class LegendRegistration
   */
  var LegendRegisterUpdate = function($scope, $q, $window, $log, $http, $timeout, i18n, MessageBoxService, LegendService, UniqueNumber) {
    /**
     * @type {LegendRegisterUpdate}
     */
    var self = this;

    /**
     * It retrieves a configuration from main window.
     *
     * @type {object}
     */
    var config = window.configuration;

    /**
     * It retrieves Global variables from main window.
     * 
     * @type {object}
     */
    var Globals = $window.globals;

    /**
     * It handles alert box display
     *
     * @type {object}
     */
    self.MessageBoxService = MessageBoxService;

    /**
     * Flag to handle if is Updating or Registering
     *
     * @type {boolean}
     */
    self.isUpdating = config.legend ? true : false;

    /**
     * It handles legend levels errors
     *
     * @type {boolean}
     */
    self.isNotValid = true;

    /**
     * It represents a terrama2 box styles
     * 
     * @type {object}
     */
    self.css = {
      boxType: "box-solid"
    };

    /**
     * It handles Legend Service Instance model
     *
     * @type {object}
     */
    self.LegendService = LegendService;

    /**
     * It contains legend instance values
     * 
     * @type {object}
     */
    self.legend = config.legend || {
      levels: [
        {
          _id: UniqueNumber(),
          name: "Default",
          isDefault: true
        },
        {
          _id: UniqueNumber(),
          name: "",
          value: ""
        }
      ]
    };

    $q.all([
      i18n.ensureLocaleIsLoaded()
    ]).then(function() {
      if(self.isUpdating) {
        self.legend.levels.sort(function(a, b) {
          if(a.level < b.level) return -1;
          if(a.level > b.level) return 1;
          return 0;
        });

        for(var j = 0, levelsLength = self.legend.levels.length; j < levelsLength; j++) {
          if(self.legend.levels[j].value == null)
            self.legend.levels[j].isDefault = true;

          self.legend.levels[j]._id = UniqueNumber();
          delete self.legend.levels[j].legend_id;
        }
      }
    });

    /**
     * It creates a new level in the legend.
     * 
     * @returns {void}
     */
    self.newLevel = function() {
      var uniqueNumberValue = UniqueNumber();

      self.legend.levels.push({
        _id: uniqueNumberValue,
        name: "",
        value: ""
      });
    };

    /**
     * It removes a given level from the legend.
     * 
     * @returns {void}
     */
    self.removeLevel = function(level) {
      for(var j = 0, levelsLength = self.legend.levels.length; j < levelsLength; j++) {
        if(self.legend.levels[j]._id === level._id) {
          self.legend.levels.splice(j, 1);
          break;
        }
      }
    };

    /**
     * Watcher for handling legend levels change. It validates if the values are numeric and are in a growing order.
     */
    $scope.$watch("ctrl.legend.levels", function() {
      if(!self.legend) return;

      var lastValue = null;
      self.legendLevelOrderError = false;
      self.isNotValid = false;

      if(self.legendLevelValueError === undefined) self.legendLevelValueError = {};
      if(self.legendLevelNameError === undefined) self.legendLevelNameError = {};

      for(var i = 0, levelsLength = self.legend.levels.length; i < levelsLength; i++) {
        if(!self.legend.levels[i].isDefault) {
          if(isNaN(self.legend.levels[i].value) || self.legend.levels[i].value === "") {
            self.legendLevelValueError[self.legend.levels[i]._id] = true;
            self.isNotValid = true;
          } else if(lastValue !== null && parseFloat(lastValue) > parseFloat(self.legend.levels[i].value)) {
            self.legendLevelOrderError = true;
            self.legendLevelValueError[self.legend.levels[i]._id] = false;
            lastValue = self.legend.levels[i].value;
            self.isNotValid = true;
          } else {
            self.legendLevelValueError[self.legend.levels[i]._id] = false;
            lastValue = self.legend.levels[i].value;
          }
        }

        if(self.legend.levels[i].name === undefined || self.legend.levels[i].name === "") {
          self.legendLevelNameError[self.legend.levels[i]._id] = true;
          self.isNotValid = true;
        } else {
          self.legendLevelNameError[self.legend.levels[i]._id] = false;
        }
      }
    }, true);

    /**
     * Helper to reset alert box instance.
     * 
     * @returns {void}
     */
    self.close = function() {
      self.MessageBoxService.reset();
    };

    /**
     * Saves the alert.
     * 
     * @returns {void}
     */
    self.save = function() {
      // Broadcasting each one terrama2 field directive validation
      $scope.$broadcast("formFieldValidation");
      // Broadcasting schema form validation
      $scope.$broadcast("schemaFormValidate");

      /**
       * Defines a common message for invalid fields
       * @type {string}
       */
      var errMessageInvalidFields = i18n.__("There are invalid fields on form");

      if(self.isNotValid) {
        self.MessageBoxService.danger(i18n.__("Legends"), errMessageInvalidFields);
        return;
      }

      $timeout(function() {
        if($scope.forms.legend.$invalid) {
          self.MessageBoxService.danger(i18n.__("Legends"), errMessageInvalidFields);
          return;
        }

        var level = 1;

        for(var i = 0, levelsLength = self.legend.levels.length; i < levelsLength; i++) {
          if(self.legend.levels[i].isDefault)
            continue;

          delete self.legend.levels[i]._id;
          self.legend.levels[i].level = level;
          level++;
        }

        var operation = self.isUpdating ? self.LegendService.update(self.legend.id, self.legend) : self.LegendService.create(self.legend);
        operation.then(function(response) {
          $log.info(response);
          $window.location.href = BASE_URL + "configuration/alerts/legends?token=" + response.token;
        }).catch(function(err) {
          $log.info(err);
          self.MessageBoxService.danger(i18n.__("Legend"), i18n.__(err));
        });
      });
    };
  };

  LegendRegisterUpdate.$inject = ["$scope", "$q", "$window", "$log", "$http", "$timeout", "i18n", "MessageBoxService", "LegendService", "UniqueNumber"];

  return LegendRegisterUpdate;
});
