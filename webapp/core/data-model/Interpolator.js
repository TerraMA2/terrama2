(function(){
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");

  function Interpolator(params) {
    // Calling base abstraction
    AbstractClass.call(this, {'class': 'Interpolator'});
    this.id = params.id;
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
    this.schedule = params.schedule || {};
    this.schedule = params.schedule || {};
    this.automaticSchedule = params.automaticSchedule || {};
  }

}());