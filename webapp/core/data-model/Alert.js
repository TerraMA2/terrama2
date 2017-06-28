(function() {
'use strict';

// Dependencies

/**
 * TerraMA² BaseClass of Data Model
 * @type {AbstractData}
 */
var BaseClass = require('./AbstractData');
var AutomaticSchedule = require("./AutomaticSchedule");
var Risk = require("./Risk");
var View = require("./View");
var ViewStyleLegend = require("./ViewStyleLegend");
var DataSeries = require("./DataSeries");
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
   * @name Alert#risk_attribute
   * @type {string}
   */
  this.risk_attribute = params.risk_attribute;
  /**
   * @name Alert#data_series_id
   * @type {string}
   */
  this.data_series_id = params.data_series_id;
  /**
   * Schedule type associated
   * @name Alert#schedule_type
   * @type {Schedule}
   */
  this.scheduleType = params.schedule_type;
  /**
   * Schedule associated
   * @name Alert#schedule
   * @type {Schedule}
   */
  this.schedule = params.schedule || {};
  /**
   * @name Alert#automatic_schedule
   * @type {object}
   */
  this.automatic_schedule = new AutomaticSchedule(params.AutomaticSchedule ? params.AutomaticSchedule.get() : params.automaticSchedule || {});
  /**
   * @name Alert#risk
   * @type {object}
   */
  this.risk = new Risk(params.Risk ? params.Risk.get() : params.risk || {});
  /**
   * @name Alert#dataSeries
   * @type {object}
   */
  this.dataSeries = new DataSeries(params.DataSeries ? params.DataSeries.get() : params.dataSeries || {});

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

  /**
   * @name Alert#view
   * @type {object}
   */
  this.view = new View(params.View ? params.View.get() : params.view || {});

  if (params.View && params.View.ViewStyleLegend){
    var legendModel = new ViewStyleLegend(Utils.extend(
      params.View.ViewStyleLegend.get(), {colors: params.View.ViewStyleLegend.ViewStyleColors ? params.View.ViewStyleLegend.ViewStyleColors.map(function(elm) { return elm.get(); }) : []}));
    legendModel.setMetadata(Utils.formatMetadataFromDB(params.View.ViewStyleLegend.ViewStyleLegendMetadata));
    this.view.setLegend(legendModel);
  }

};

/**
 * It sets automatic schedule data.
 * @param {Sequelize.Model[]|Object[]}
 */
Alert.prototype.setAutomaticSchedule = function(automaticSchedule) {
  if (automaticSchedule.AutomaticSchedule) {
    this.automatic_schedule = new AutomaticSchedule(automaticSchedule.AutomaticSchedule.get() || {});
  } else {
    this.automatic_schedule = automaticSchedule || {};
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
    risk_attribute: this.risk_attribute,
    schedule_type: this.scheduleType,
    schedule: this.schedule instanceof BaseClass ? this.schedule.toObject() : {},
    automatic_schedule: this.automatic_schedule instanceof BaseClass ? this.automatic_schedule.toObject() : this.automatic_schedule,
    risk: this.risk instanceof BaseClass ? this.risk.toObject() : this.risk,
    dataSeries: this.dataSeries instanceof BaseClass ? this.dataSeries.toObject() : this.dataSeries,
    additional_data: this.additional_data,
    notifications: this.notifications,
    report_metadata: this.report_metadata,
    view: this.view instanceof BaseClass ? this.view.toObject() : this.view,
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
    risk_attribute: this.risk_attribute,
    risk: this.risk instanceof BaseClass ? this.risk.toService() : this.risk,
    additional_data: additionalDataList,
    notifications: notificationList,
    report_metadata: reportMetadataCopy,
    schedule: this.schedule instanceof BaseClass ? this.schedule.toObject() : {}
  });
}

module.exports = Alert;

} ());