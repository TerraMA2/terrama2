(function() {
  'use strict';

  angular.module('terrama2.components.messagebox.services', ['terrama2'])
    /**
     * It defines alert level of component.
     * @type {Object}
     */
    .constant("AlertLevel", {
      /**
       * It defines info level. It will display light-blue component
       * @type {string}
       */
      "INFO": "alert-info",
      "SUCCESS": "alert-success",
      "WARNING": "alert-warning",
      "DANGER": "alert-danger"
    })
    
    .service('MessageBoxService', MessageBoxService); 

  /**
   * It stores a message box service context.
   * 
   * @class MessageBoxService
   * 
   * @param {Object} AlertLevel
   */
  function MessageBoxService(AlertLevel) {
    /**
     * It defines const values of AlertLevel
     * @type {AlertLevel}
     */
    this.Levels = AlertLevel;

    /**
     * It defines a alert box structure for handling directive
     * @type {Object}
     */
    this.alertBox = {
      title: "",
      message: "",
      display: false,
      level: this.Levels.INFO,
      extra: {}
    };
  }

  MessageBoxService.$inject = ["AlertLevel"]

  /**
   * It resets current alertBox object to initial state
   * @returns {void}
   */
  MessageBoxService.prototype.reset = function() {
    this.alertBox.title = "";
    this.alertBox.message = "";
    this.alertBox.display = false;
    this.alertBox.level = this.Levels.INFO;
    this.alertBox.extra = {};
  };

  /**
   * It fills out alert box structure. 
   * 
   * @private It should not be accessed directly. Use .info, .danger, .success or .warning instead.
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   * @param {boolean} display - flag to display message box
   * @param {AlertLevel} level - A message box level
   * @param {object} extra - Extra propertie to show in box
   */
  MessageBoxService.prototype.$configure = function(title, message, display, level, extra) {
    this.alertBox.title = title;
    this.alertBox.message = message;
    this.alertBox.display = display;
    this.alertBox.level = level;
    if (extra){
      this.alertBox.extra = extra;
    }
  };

  /**
   * It prepares a message box with info level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   * @param {object} extra - Extra propertie to show in box
   */
  MessageBoxService.prototype.info = function(title, message, extra) {
    this.$configure(title, message, true, this.Levels.INFO, extra);
  };

  /**
   * It prepares a message box with success level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   * @param {object} extra - Extra propertie to show in box
   */
  MessageBoxService.prototype.success = function(title, message, extra) {
    this.$configure(title, message, true, this.Levels.SUCCESS, extra);
  };

  /**
   * It prepares a message box with warning level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   * @param {object} extra - Extra propertie to show in box
   */
  MessageBoxService.prototype.warning = function(title, message, extra) {
    this.$configure(title, message, true, this.Levels.WARNING, extra);
  };

  /**
   * It prepares a message box with danger level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   * @param {object} extra - Extra propertie to show in box
   */
  MessageBoxService.prototype.danger = function(title, message, extra) {
    this.$configure(title, message, true, this.Levels.DANGER, extra);
  };

  /**
   * It retrieves current alert box icon.
   * 
   * @returns {string} String icon
   */
  MessageBoxService.prototype.getIcon = function() {
    switch(this.alertBox.level) {
      case this.Levels.WARNING:
        return "fa-exclamation-triangle";
      case this.Levels.DANGER:
        return "fa-times";
      case this.Levels.SUCCESS:
        return "fa-check";
      default:
        return "fa-info";
    }
  };
}());