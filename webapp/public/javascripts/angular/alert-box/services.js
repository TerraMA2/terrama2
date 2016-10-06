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
      level: this.Levels.INFO
    };
  }

  /**
   * It resets current alertBox object to initial state
   * @returns {void}
   */
  MessageBoxService.prototype.reset = function() {
    this.alertBox.title = "";
    this.alertBox.message = "";
    this.alertBox.display = false;
    this.alertBox.level = this.Levels.INFO;
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
   */
  MessageBoxService.prototype.$configure = function(title, message, display, level) {
    this.alertBox.title = title;
    this.alertBox.message = message;
    this.alertBox.display = display;
    this.alertBox.level = level;
  };

  /**
   * It prepares a message box with info level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   */
  MessageBoxService.prototype.info = function(title, message) {
    this.$configure(title, message, true, this.Levels.INFO);
  };

  /**
   * It prepares a message box with success level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   */
  MessageBoxService.prototype.success = function(title, message) {
    this.$configure(title, message, true, this.Levels.SUCCESS);
  };

  /**
   * It prepares a message box with warning level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   */
  MessageBoxService.prototype.warning = function(title, message) {
    this.$configure(title, message, true, this.Levels.WARNING);
  };

  /**
   * It prepares a message box with danger level
   * 
   * @param {string} title - A message box title
   * @param {string} message - A message box body
   */
  MessageBoxService.prototype.danger = function(title, message) {
    this.$configure(title, message, true, this.Levels.DANGER);
  };

}());