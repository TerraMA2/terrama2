(function() {
'use strict';

// Dependencies

/**
 * TerraMA² BaseClass of Data Model
 * @type {AbstractData}
 */
var BaseClass = require('./AbstractData');
var ConditionalSchedule = require("./ConditionalSchedule");
var ConditionalSchedule = require("./Risk");
/**
 * TerraMA² Global Utility module
 * @type {Utils}
 */
var Utils = require("./../Utils");

/**
 * TerraMA² Alert Data Model representation
 * 
 * @class Alert
 */
var Alert = function(params) {
  BaseClass.call(this, {'class': 'Alert'});
  /**
   * Alert#id
   * @type {number}
   */
  this.id = params.id;
  /**
   * @name Alert#project_id
   * @type {number}
   */
  this.project_id = params.project_id;
  /**
   * @name Alert#instance_id
   * @type {number}
   */
  this.instance_id = params.instance_id;
  /**
   * @name Alert#active
   * @type {boolean}
   */
  this.active = params.active;
  /**
   * @name Alert#name
   * @type {string}
   */
  this.name = params.name;
  /**
   * @name Alert#name
   * @type {string}
   */
  this.description = params.description;
  /**
   * @name Alert#risk_attribute
   * @type {string}
   */
  this.risk_attribute = params.risk_attribute;
  /**
   * @name Alert#conditional_schedule
   * @type {object}
   */
  this.conditional_schedule = new ConditionalSchedule(params.ConditionalSchedule ? params.ConditionalSchedule.get() : params.conditionalSchedule || {});
  /**
   * @name Alert#conditional_schedule
   * @type {object}
   */
  this.risk = new Risk(params.Risk ? params.Risk.get() : params.risk || {});

  /**
   * @name Alert#report_metadata
   * @type {object}
   */
  this.report_metadata = params.ReportMetadata ? params.ReportMetadata.get() : params.reportMetadata || {};

  // Setting additional data
  this.setAlertAdditionalData(params.AlertAdditionalData || params.alertAdditionalData);
  // Setting notifications
  this.setAlertNotifications(params.AlertNotifications || params.alertNotifications);
  
};
/**
 * It sets additional data.
 * @param {Sequelize.Model[]|Object[]}
 */
Alert.prototype.setAlertAdditionalData = function(AlertAdditionalDatas) {
  var output = [];
  AlertAdditionalDatas.forEach(function(AlertAdditionalData) {
    var obj = AlertAdditionalData;
    if (Utils.isFunction(AlertAdditionalData.get)) {
      obj = AlertAdditionalData.get();
    }
    output.push(obj);
  });
  this.alert_additional_data = output;
};

/**
 * It sets alert notifications to cache.
 * @todo Improve this doc
 * @param {Sequelize.Model[]|Object[]}
 */
Alert.prototype.setAlertNotifications = function(AlertNotifications) {
  var output = [];
  AlertNotifications.forEach(function(AlertNotification) {
    var obj = AlertNotification;
    if (Utils.isFunction(AlertNotification.get)) {
      obj = AlertNotification.get();
    }
    output.push(obj);
  });
  this.alert_notifications = output;
};

Alert.prototype = Object.create(BaseClass.prototype);
Alert.prototype.constructor = Alert;

Alert.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    instance_id: this.class,
    active: this.active,
    name: this.name,
    description: this.description,
    conditional_schedule: this.conditional_schedule instanceof BaseClass ? this.conditional_schedule.toObject() : this.conditional_schedule,
    risk: this.risk instanceof BaseClass ? this.risk.toObject() : this.risk,
    alert_additional_data: this.alert_additional_data,
    alert_notifications: this.alert_notifications
  });
};

Alert.prototype.rawObject = function() {
  var toObject = this.toObject();
  return toObject;
};

module.exports = Alert;

} ());