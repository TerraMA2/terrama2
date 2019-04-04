
'use strict';
  
  // Dependency

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require("./AbstractData");
  var Schedule = require("./Schedule");
  var AutomaticSchedule = require("./AutomaticSchedule");
/**
 * TerraMA² Storage representation
 * 
 * @constructor
 * @inherits AbstractData
 */
var Storage = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Storage'});
  this.id = params.id;
  this.name= params.name,
  this.description= params.description,
  this.active = params.active;
  this.erase_all = params.erase_all,
  this.project_id = params.project_id;
  this.filter = params.filter,
  this.service_instance_id = params.service_instance_id;
  this.data_series_id = params.data_series_id;
  this.data_provider = params.data_provider;
  this.schedule_type = params.schedule_type;
  this.keep_data = params.keep_data,
  this.keep_data_unit = params.keep_data_unit,
  this.backup = params.backup,
  this.uri = params.uri,
  this.zip = params.zip

  if (params.Schedule || params.schedule){
      this.schedule = new Schedule(params.Schedule ? params.Schedule.get() : params.schedule);
  } else {
      this.schedule = {};
  }

  this.automaticSchedule = new AutomaticSchedule(params.AutomaticSchedule ? params.AutomaticSchedule.get() : params.automaticSchedule || {});
};

// javascript inherits model
Storage.prototype = Object.create(BaseClass.prototype);
Storage.prototype.constructor = Storage;

Storage.prototype.rawObject = function() {
  var toObject = this.toObject();

  toObject.dataSeries = this.dataSeries instanceof BaseClass ? this.dataSeries.toObject() : this.dataSeries;
  toObject.dataProvider = this.dataProvider instanceof BaseClass ? this.dataProvider.toObject() : this.dataProvider;
  return toObject;
};

Storage.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      description: this.description,
      active: this.active,
      erase_all: this.erase_all,
      keep_data: this.keep_data,
      keep_data_unit: this.keep_data_unit,
      filter: this.filter,
      backup: this.backup,
      uri: this.uri,
      zip: this.zip,
      project_id: this.projectId,
      dataSeries: this.dataSeries instanceof BaseClass ? this.dataSeries.toObject() : {},
      dataProvider: this.dataProvider instanceof BaseClass ? this.dataProvider.toObject() : {},
      schedule_type: this.scheschedule_typedule instanceof BaseClass ? this.schedschedule_typeule.toObject() : {},
      automatic_schedule: this.automaticSchedule instanceof BaseClass ? this.automaticSchedule.toObject() : {},
      service_instance_id: this.serviceInstanceId
  });
};
