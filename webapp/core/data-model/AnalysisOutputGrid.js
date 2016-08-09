"use strict";

var BaseClass = require("./AbstractData");
var Utils = require('./../Utils');

var AnalysisOutputGrid = module.exports = function(params) {
  BaseClass.call(this, {'class': "AnalysisOutputGrid"});

  this.id = params.id;
  if (params.InterpolationMethod) {
    this.interpolationMethod = params.InterpolationMethod.get();
  } else {
    this.interpolationMethod = params.interpolationMethod || params.interpolation_method;
  }
  this.interpolationDummy = params.interpolationDummy || params.interpolation_dummy;
  this.srid = params.srid;
  this.analysis = params.analysis || {id: params.analysis_id};
  this.areaOfInterestType = params.areaOfInterestType || params.area_of_interest_type;
  this.areaOfInterestBox = params.areaOfInterestBox || params.area_of_interest_box || params.interest_box;
  this.areaOfInterestDataSeries = params.areaOfInterestDataSeries || {id: params.area_of_interest_data_series_id};
  this.resolutionX = params.resolutionX || params.resolution_x;
  this.resolutionY = params.resolutionY || params.resolution_y;
  this.resolutionType = params.resolutionType || params.resolution_type;
  this.resolutionDataSeries = params.resolutionDataSeries || {id: params.resolution_data_series_id};
};

AnalysisOutputGrid.prototype = Object.create(BaseClass.prototype);
AnalysisOutputGrid.prototype.constructor = AnalysisOutputGrid;


AnalysisOutputGrid.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    interpolation_dummy: this.interpolationDummy ? Number(this.interpolationDummy) : this.interpolationDummy,
    interpolation_method: this.interpolationMethod,
    srid: this.srid,
    analysis_id: this.analysis.id,
    area_of_interest_type: this.areaOfInterestType,
    area_of_interest_data_series_id: this.areaOfInterestDataSeries.id,
    area_of_interest_box: this.areaOfInterestBox,
    resolution_x: this.resolutionX,
    resolution_y: this.resolutionY,
    resolution_type: this.resolutionType,
    resolution_data_series_id: this.resolutionDataSeries.id
  });
};

AnalysisOutputGrid.prototype.rawObject = function() {
  return this.toObject();
};
