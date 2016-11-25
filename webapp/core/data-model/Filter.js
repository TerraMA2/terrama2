'use strict';

var AbstractClass = require("./AbstractData");
var Utils = require('./../Utils');

function Filter(params) {
  AbstractClass.call(this, {'class': 'Filter'});

  this.id = params.id;
  this.frequency = params.frequency;
  this.frequency_unit = params.frequency_unit;
  this.discard_before = params.discard_before;
  this.discard_after = params.discard_after;
  this.region = params.region;
  this.region_wkt = params.region_wkt;
  this.by_value = params.by_value;
  this.crop_raster = params.crop_raster;
  this.collector_id = params.collector_id;
}

Filter.prototype = Object.create(AbstractClass.prototype);
Filter.prototype.constructor = Filter;

Filter.prototype.rawObject = function() {
  var obj = this.toObject();
  obj.region = this.region;
  return obj;
};

Filter.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    frequency: this.frequency,
    frequency_unit: this.frequency_unit,
    discard_before: this.discard_before instanceof Date ? Utils.formatDateToTimezone(this.discard_before) : this.discard_before,
    discard_after: this.discard_after instanceof Date ? Utils.formatDateToTimezone(this.discard_after) : this.discard_after,
    region: this.region_wkt || this.region,
    by_value: this.by_value,
    crop_raster: this.crop_raster,
    collector_id: this.collector_id
  });
};

module.exports = Filter;
