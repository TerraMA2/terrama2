(function(){
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");

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
    this.resolution_x =  params.resolution_x;
    this.resolution_y = params.resolution_y;
    this.srid = params.srid;
    this.schedule_type = params.schedule_type;
    this.schedule = params.schedule || {};
    this.automaticSchedule = params.automaticSchedule || {};
    this.dataSeriesOutput = params.dataSeriesOutput || {};
    this.dataSeriesInput = params.dataSeriesInput || {};
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
        meta[element.key] = element.value;
      });
    } else {
      for(var key in metadata) {
        if (metadata.hasOwnProperty(key)) {
          meta[key] = metadata[key];
        }
      }
    }
  
    this.metadata = meta;
  };

  Interpolator.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      active: this.active,
      project_id: params.project_id,
      service_instance_id: params.service_instance_id,
      data_series_input: params.data_series_input,
      data_series_output: params.data_series_output,
      schedule_type: params.schedule_type,
      bounding_rect: params.bounding_rect,
      interpolation_attribute: params.interpolation_attribute,
      resolution_x: params.resolution_x,
      resolution_y: params.resolution_y,
      srid: params.srid,
      metadata: this.metadata
    });
  };
  module.exports = Interpolator;
}());