var BaseClass = require('./AbstractData');
var Utils = require("./../Utils");


var Schedule = function(params) {
  BaseClass.call(this, {'class': 'Schedule'});

  this.id = params.id;
  this.frequency = params.frequency;
  this.frequency_unit = params.frequency_unit;
  this.frequency_start_time = params.frequency_start_time;
  this.schedule = params.schedule;
  this.schedule_time = params.schedule_time;
  this.schedule_unit = params.schedule_unit;
  this.schedule_retry = params.schedule_retry;
  this.schedule_retry_unit = params.schedule_retry_unit;
  this.schedule_timeout = params.schedule_timeout;
  this.schedule_timeout_unit = params.schedule_timeout_unit;
};

Schedule.prototype = Object.create(BaseClass.prototype);
Schedule.prototype.constructor = Schedule;

Schedule.prototype.toObject = function() {
  var frequency_start_time = null;
  if (this.frequency_start_time) {
    frequency_start_time = Utils.formatDateToTimezone(new Date(this.frequency_start_time));
  }
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    frequency: this.frequency,
    frequency_unit: this.frequency_unit,
    frequency_start_time: frequency_start_time,
    schedule: this.schedule,
    schedule_time: this.schedule_time,
    schedule_unit: this.schedule_unit,
    schedule_retry: this.schedule_retry,
    schedule_retry_unit: this.schedule_retry_unit,
    schedule_timeout: this.schedule_timeout,
    schedule_timeout_unit: this.schedule_timeout_unit
  })
};

module.exports = Schedule;
