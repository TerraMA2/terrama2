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
   * TerraMA² ConditionalSchedule Data Model representation
   * 
   * @class ConditionalSchedule
   */
  var ConditionalSchedule = function(params) {
    BaseClass.call(this, {'class': 'ConditionalSchedule'});
    /**
     * ConditionalSchedule Identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * ConditionalSchedule Data series ids
     * @type {array}
     */
    this.data_ids = params.data_ids || [];
  };

  ConditionalSchedule.prototype = Object.create(BaseClass.prototype);
  ConditionalSchedule.prototype.constructor = ConditionalSchedule;

  ConditionalSchedule.prototype.toObject = function() {
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      data_ids: this.data_ids,
    });
  };

  ConditionalSchedule.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  module.exports = ConditionalSchedule;

} ());