(function() {
'use strict';

// Dependencies

/**
 * TerraMA² BaseClass of Data Model
 * @type {AbstractData}
 */
var BaseClass = require('./AbstractData');

/**
 * TerraMA² AlertAttachedView Data Model representation
 * 
 * @class AlertAttachedView
 */
var AlertAttachedView = function(params) {
  BaseClass.call(this, {'class': 'AlertAttachedView'});
  /**
   * AlertAttachedView#id
   * @type {number}
   */
  this.id = params.id;
  /**
   * @name AlertAttachedView#layer_order
   * @type {number}
   */
  this.layer_order = params.layer_order;
  /**
   * @name AlertAttachedView#alert_id
   * @type {number}
   */
  this.alert_id = params.alert_id;
  /**
   * @name AlertAttachedView#view_id
   * @type {number}
   */
  this.view_id = params.view_id;
};

AlertAttachedView.prototype = Object.create(BaseClass.prototype);
AlertAttachedView.prototype.constructor = AlertAttachedView;

AlertAttachedView.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    layer_order: this.layer_order,
    alert_id: this.alert_id,
    view_id: this.view_id
  });
};

AlertAttachedView.prototype.rawObject = function() {
  var toObject = this.toObject();
  return toObject;
};

AlertAttachedView.prototype.toService = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    layer_order: this.layer_order,
    alert_id: this.alert_id,
    view_id: this.view_id
  });
}

module.exports = AlertAttachedView;

} ());