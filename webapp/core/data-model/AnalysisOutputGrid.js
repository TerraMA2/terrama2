"use strict";

// dependencies
var logger = require("./../Logger");
var BaseClass = require("./AbstractData");
var Utils = require('./../Utils');

/**
 * AnalysisOutputGrid representation
 * 
 * @class AnalysisOutputGrid
 * @param {AnalysisOutputGrid | Object} params - A params to fill analysis output grid 
 */
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

  this.setAreaOfInterestBox(params.areaOfInterestBox || params.area_of_interest_box);

  this.areaOfInterestBoxWKT = params.interest_box || null;
  this.areaOfInterestDataSeries = params.areaOfInterestDataSeries || {id: params.area_of_interest_data_series_id};
  this.resolutionX = params.resolutionX || params.resolution_x;
  this.resolutionY = params.resolutionY || params.resolution_y;
  this.resolutionType = params.resolutionType || params.resolution_type;
  this.resolutionDataSeries = params.resolutionDataSeries || {id: params.resolution_data_series_id};
};

// javascript inherits model
AnalysisOutputGrid.prototype = Object.create(BaseClass.prototype);
AnalysisOutputGrid.prototype.constructor = AnalysisOutputGrid;

/**
 * It prepares and set area of interest box in property
 * 
 * @param {string | Object} areaOfInterestBox - An area of interest box retrieved from database. It may be a GeoJSON string to be parsed or a javascript object
 */
AnalysisOutputGrid.prototype.setAreaOfInterestBox = function(areaOfInterestBox) {
  if (Utils.isString(areaOfInterestBox)) {
    try {
      this.areaOfInterestBox = JSON.parse(areaOfInterestBox);
    } catch (e) {
      logger.debug("Error during AreaOfInterestBox parse. " + e.toString());
      this.areaOfInterestBox = areaOfInterestBox;
    }
  } else {
    this.areaOfInterestBox = areaOfInterestBox;
  }
};

AnalysisOutputGrid.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    interpolation_dummy: this.interpolationDummy ? Number(this.interpolationDummy) : this.interpolationDummy,
    interpolation_method: this.interpolationMethod,
    srid: (this.srid || Utils.isNumber(this.srid)) ? this.srid : null,
    analysis_id: this.analysis.id,
    area_of_interest_type: (this.areaOfInterestType || Utils.isNumber(this.areaOfInterestType)) ? this.areaOfInterestType : null,
    area_of_interest_data_series_id: this.areaOfInterestDataSeries.id || null,
    area_of_interest_box: this.areaOfInterestBoxWKT || this.areaOfInterestBox || null,
    resolution_x: (this.resolutionX || Utils.isNumber(this.resolutionX)) ? this.resolutionX : null,
    resolution_y: (this.resolutionY || Utils.isNumber(this.resolutionY)) ? this.resolutionY : null,
    resolution_type: this.resolutionType || null,
    resolution_data_series_id: this.resolutionDataSeries.id || null
  });
};

AnalysisOutputGrid.prototype.rawObject = function() {
  var toObj = this.toObject();
  toObj.area_of_interest_box = this.areaOfInterestBox;
  return toObj;
};