(function() {
'use strict';

// Dependencies

/**
 * TerraMA² BaseClass of Data Model
 * @type {AbstractData}
 */
var BaseClass = require('./AbstractData');
var ConditionalSchedule = require("./ConditionalSchedule");
var Legend = require("./Legend");
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
   * @name Alert#service_instance_id
   * @type {number}
   */
  this.service_instance_id = params.service_instance_id;
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
   * @name Alert#legend_attribute
   * @type {string}
   */
  this.legend_attribute = params.legend_attribute;
  /**
   * @name Alert#data_series_id
   * @type {string}
   */
  this.data_series_id = params.data_series_id;
  /**
   * @name Alert#conditional_schedule
   * @type {object}
   */
  this.conditional_schedule = new ConditionalSchedule(params.ConditionalSchedule ? params.ConditionalSchedule.get() : params.conditionalSchedule || {});
  /**
   * @name Alert#legend
   * @type {object}
   */
  this.legend = new Legend(params.Legend ? params.Legend.get() : params.legend || {});

  /**
   * @name Alert#report_metadata
   * @type {object}
   */
  this.report_metadata = params.reportMetadata || [];

  /**
   * @name Alert#additional_data
   * @type {object}
   */
  this.additional_data = params.additionalData || [];

  /**
   * @name Alert#notifications
   * @type {object}
   */
  this.notifications = params.notifications || [];
  
};

/**
 * It sets conditional schedule data.
 * @param {Sequelize.Model[]|Object[]}
 */
Alert.prototype.setConditionalSchedule = function(conditionalSchedule) {
  if (conditionalSchedule.ConditionalSchedule) {
    this.conditional_schedule = new ConditionalSchedule(conditionalSchedule.ConditionalSchedule.get() || {});
  } else {
    this.conditional_schedule = conditionalSchedule || {};
  }
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
    service_instance_id: this.service_instance_id,
    active: this.active,
    name: this.name,
    description: this.description,
    data_series_id: this.data_series_id,
    legend_attribute: this.legend_attribute,
    conditional_schedule: this.conditional_schedule instanceof BaseClass ? this.conditional_schedule.toObject() : this.conditional_schedule,
    legend: this.legend instanceof BaseClass ? this.legend.toObject() : this.legend,
    additional_data: this.additional_data,
    notifications: this.notifications,
    report_metadata: this.report_metadata
  });
};

Alert.prototype.rawObject = function() {
  var toObject = this.toObject();
  return toObject;
};

Alert.prototype.toService = function() {
  var additionalDataList = [];
  if (this.additional_data && this.additional_data.length > 0){
    this.additional_data.forEach(function(addData){
      if (addData.attributes){
        addData.attributes = addData.attributes.split(';');
      }
      additionalDataList.push(addData);
    });
  }
  var notificationList = [];
  if (this.notifications && this.notifications.length > 0){
    this.notifications.forEach(function(notification){
      if (notification.recipients){
        notification.recipients = notification.recipients.split(";");
        delete notification.id;
        delete notification.alert_id;
      }
      notificationList.push(notification);
    });
  }

  var reportMetadataCopy = Object.assign({}, this.report_metadata);
  delete reportMetadataCopy.id;
  delete reportMetadataCopy.alert_id;

  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    service_instance_id: this.service_instance_id,
    active: this.active,
    name: this.name,
    description: this.description,
    data_series_id: this.data_series_id,
    legend_attribute: this.legend_attribute,
    legend: this.legend instanceof BaseClass ? this.legend.toService() : this.legend,
    additional_data: additionalDataList,
    notifications: notificationList,
    report_metadata: reportMetadataCopy
  });
}

module.exports = Alert;

} ());