var BaseClass = require("./AbstractData");
var Schedule = require("./Schedule");
var Filter = require('./Filter');

var Collector = module.exports = function(params) {
  BaseClass.call(this, {'class': 'Collector'});
  this.id = params.id;
  this.project_id = params.project_id;
  this.service_instance_id = params.service_instance_id;
  this.input_data_series = params.data_series_input;
  this.output_data_series = params.data_series_output;

  if (params.CollectorInputOutputs)
    this.setInputOutputMap(params.CollectorInputOutputs);
  else
    this.input_output_map = params.input_output_map || [];

  if (params.Schedule)
    this.schedule = new Schedule(params.Schedule.get() || {});
  else
    this.schedule = params.schedule || {};

  if (params.Filter)
    this.filter = new Filter(params.Filter.get() || {});
  else
    this.filter = params.filter || {};

  this.intersection = params.intersection || {};
  this.active = params.active;
};

Collector.prototype = Object.create(BaseClass.prototype);
Collector.prototype.constructor = Collector;

Collector.prototype.setInputOutputMap = function (inputOutputModel) {
  var output = [];
  inputOutputModel.forEach(function(element) {
    // TODO: fix it
    if (typeof element.get === "function") {
      output.push({input: element.input_dataset, output: element.output_dataset});
    }
    else
      output.push(element);
  })
  this.input_output_map = output;
};

Collector.prototype.rawObject = function () {
  var obj = this.toObject();
  obj.schedule = this.schedule.toObject();
  obj.filter = this.filter instanceof BaseClass ? this.filter.toObject() : this.filter;
  return obj;
};

Collector.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    project_id: this.project_id,
    service_instance_id: this.service_instance_id,
    input_data_series: this.input_data_series,
    output_data_series: this.output_data_series,
    input_output_map: this.input_output_map || [],
    schedule: this['schedule'] instanceof BaseClass ? this['schedule'].toObject() : this['schedule'],
    filter: this['filter'] instanceof BaseClass ? this.filter.toObject() : this.filter,
    intersection: this.intersection,
    active: this.active
  });
};
