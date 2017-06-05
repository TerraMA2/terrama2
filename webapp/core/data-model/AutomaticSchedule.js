(function() {
  'use strict';

  // Dependencies

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require('./AbstractData');
  /**
   * TerraMA² Global Utility module
   * @type {Utils}
   */
  var Utils = require("./../Utils");

  /**
   * TerraMA² AutomaticSchedule Data Model representation
   * 
   * @class AutomaticSchedule
   */
  var AutomaticSchedule = function(params) {
    BaseClass.call(this, {'class': 'AutomaticSchedule'});
    /**
     * AutomaticSchedule Identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * AutomaticSchedule Data series ids
     * @type {array}
     */
    this.data_ids = params.data_ids || [];
  };

  AutomaticSchedule.prototype = Object.create(BaseClass.prototype);
  AutomaticSchedule.prototype.constructor = AutomaticSchedule;

  AutomaticSchedule.prototype.toObject = function() {
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      data_ids: this.data_ids,
    });
  };

  AutomaticSchedule.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  module.exports = AutomaticSchedule;

} ());