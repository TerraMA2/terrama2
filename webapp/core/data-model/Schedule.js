(function() {
  'use strict';

  // Dependencies

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require('./AbstractData');
  /**
   * TerraMA² Global Utility module
   * @type {Utils}
   */
  var Utils = require("./../Utils");
  /**
   * TerraMA² Reprocessing Historial Data
   * @type {ReprocessingHistoricalData}
   */
  var ReprocessingHistoricalData = require("./ReprocessingHistoricalData");

  /**
   * TerraMA² Schedule Data Model representation
   * 
   * @class Schedule
   */
  var Schedule = function(params) {
    BaseClass.call(this, {'class': 'Schedule'});
    /**
     * Schedule Identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * Schedule frequency value
     * @type {number}
     */
    this.frequency = params.frequency || 0;
    /**
     * Schedule frequency unit (minutes,hours,seconds,weekly)
     * @type {string}
     */
    this.frequency_unit = params.frequency_unit || "";
    /**
     * Defines TerraMA² Start Time frequency
     * @type {string}
     */
    this.frequency_start_time = params.frequency_start_time || "";
    /**
     * Defines TerraMA² Schedule value
     * @type {number}
     */
    this.schedule = params.schedule || 0;
    /**
     * Defines TerraMA² Schedule Time. 
     * @type {string}
     */
    this.schedule_time = params.schedule_time || "";
    /**
     * Defines TerraMA² Schedule Unit values.
     * @type {string}
     */
    this.schedule_unit = params.schedule_unit || "";
    this.schedule_retry = params.schedule_retry || 0;
    this.schedule_retry_unit = params.schedule_retry_unit || "";
    this.schedule_timeout = params.schedule_timeout || 0;
    this.schedule_timeout_unit = params.schedule_timeout_unit || "";
    /**
     * @type {ReprocessingHistoricalData}
     */
    this.historicalData = null;
  
    if (params.ReprocessingHistoricalDatum) {
      this.setHistoricalData(params.ReprocessingHistoricalDatum);
    } else {
      this.setHistoricalData(params.historicalData || params.historical);
    }
  };

  Schedule.prototype = Object.create(BaseClass.prototype);
  Schedule.prototype.constructor = Schedule;

  /**
   * It creates and sets a ReprocessingHistoricalData to historicalData
   *
   * @param {Object | ReprocessingHistoricalData}
   */
  Schedule.prototype.setHistoricalData = function(historicalData) {
    if (historicalData instanceof BaseClass) {
      this.historicalData = historicalData;
    } else {
      if (historicalData && !Utils.isEmpty(historicalData)) {
        this.historicalData = new ReprocessingHistoricalData(historicalData);
      } else {
        this.historicalData = {};
      }
    }
  };

  Schedule.prototype.toObject = function() {
    var historicalData = this.historicalData instanceof BaseClass ? this.historicalData.toObject() : this.historicalData;
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      frequency: this.frequency,
      frequency_unit: this.frequency_unit,
      frequency_start_time: this.frequency_start_time,
      schedule: this.schedule,
      schedule_time: this.schedule_time,
      schedule_unit: this.schedule_unit,
      schedule_retry: this.schedule_retry,
      schedule_retry_unit: this.schedule_retry_unit,
      schedule_timeout: this.schedule_timeout,
      schedule_timeout_unit: this.schedule_timeout_unit,
      reprocessing_historical_data: Utils.isEmpty(historicalData) ? null : historicalData
    });
  };

  Schedule.prototype.rawObject = function() {
    var toObject = this.toObject();
    toObject.frequency_start_time = this.frequency_start_time;
    return toObject;
  };

  module.exports = Schedule;

} ());