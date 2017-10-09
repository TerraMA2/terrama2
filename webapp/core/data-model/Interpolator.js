(function(){
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");
  var Schedule = require("./Schedule");
  var AutomaticSchedule = require("./AutomaticSchedule");

  function Interpolator(params) {
    // Calling base abstraction
    AbstractClass.call(this, {'class': 'Interpolator'});
    this.id = params.id;
    this.active = params.active;
    this.project_id = params.project_id;
    this.service_instance_id = params.service_instance_id;
    this.data_series_input = params.data_series_input;
    this.data_series_output = params.data_series_output;
    this.schedule_type = params.schedule_type;
    this.bounding_rect = params.bounding_rect;
    this.interpolation_attribute = params.interpolation_attribute;
    this.interpolator_strategy = params.interpolator_strategy;
    this.resolution_x =  params.resolution_x;
    this.resolution_y = params.resolution_y;
    this.srid = params.srid;
    this.schedule_type = params.schedule_type;

    if (params.Schedule || params.schedule){
      this.schedule = new Schedule(params.Schedule ? params.Schedule.get() : params.schedule);
    } else {
      this.schedule = {};
    }

    this.automaticSchedule = new AutomaticSchedule(params.AutomaticSchedule ? params.AutomaticSchedule.get() : params.automaticSchedule || {});

    if (params.InterpolatorMetadata){
      this.setMetadata(params.InterpolatorMetadata);
    } else {
      this.metadata = params.interpolator_metadata;
    }
  }

  Interpolator.prototype.setMetadata = function(metadata) {
    var meta = {};
    if (metadata instanceof Array) {
      // array of sequelize model
      metadata.forEach(function(element) {
        meta[element.key] = Number(element.value);
      });
    } else {
      for(var key in metadata) {
        if (metadata.hasOwnProperty(key)) {
          meta[key] = Number(metadata[key]);
        }
      }
    }
  
    this.metadata = meta;
  };

  Interpolator.prototype.setInputOutputDataSeries = function(input_ds, output_ds){
    this.dataSeriesInput = input_ds;
    this.dataSeriesOutput = output_ds;
  }

  Interpolator.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  Interpolator.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      active: this.active,
      project_id: this.project_id,
      service_instance_id: this.service_instance_id,
      data_series_input: this.data_series_input,
      data_series_output: this.data_series_output,
      dataSeriesInput: this.dataSeriesInput,
      dataSeriesOutput: this.dataSeriesOutput,
      schedule_type: Number(this.schedule_type),
      bounding_rect: this.bounding_rect,
      interpolation_attribute: this.interpolation_attribute,
      interpolator_strategy: Number(this.interpolator_strategy),
      resolution_x: Number(this.resolution_x),
      resolution_y: Number(this.resolution_y),
      srid: Number(this.srid),
      metadata: this.metadata,
      automatic_schedule: this.automaticSchedule instanceof AbstractClass ? this.automaticSchedule.rawObject() : this.automaticSchedule,
      schedule: this.schedule instanceof AbstractClass ? this.schedule.rawObject() : this.schedule
    });
  };

  Interpolator.prototype.toService = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      active: this.active,
      project_id: this.project_id,
      bounding_rect: this.bounding_rect,
      input_data_series: this.data_series_input,
      output_data_series: this.data_series_output,
      interpolation_attribute: this.interpolation_attribute,
      interpolator_strategy: this.interpolator_strategy,
      resolution_x: this.resolution_x,
      resolution_y: this.resolution_y,
      service_instance_id: this.service_instance_id,
      srid: this.srid,
      number_of_neighbors: this.metadata.number_of_neighbors,
      power_factor: this.metadata.power_factor
    });
  };
  module.exports = Interpolator;
}());