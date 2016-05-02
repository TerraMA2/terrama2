var BaseClass = require("./AbstractData");

var Collector = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Collector'});
  this.id = params.id;
  this.project_id = params.project_id;
  this.service_instance_id = params.service_instance_id;
  this.input_data_series = params.data_series_input;
  this.output_data_series = params.data_series_output;
  this.input_output_map = params.input_output_map || [];
  this.schedule = params.schedule || {};
  this.intersection = params.intersection;
  this.active = params.active;
};

Collector.prototype = Object.create(BaseClass.prototype);
Collector.prototype.constructor = Collector;

Collector.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    service_instance_id: this.service_instance_id,
    input_data_series: this.input_data_series,
    output_data_series: this.output_data_series,
    input_output_map: this.input_output_map || [],
    schedule: this['schedule'],
    intersection: this.intersection,
    active: this.active
  });
};