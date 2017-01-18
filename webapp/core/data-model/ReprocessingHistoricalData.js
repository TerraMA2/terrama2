var AbstractClass = require("./AbstractData");
var Utils = require("./../Utils");

/**
 * It represents an Analysis Reprocessing Historical Data
 * 
 * @class ReprocessingHistoricalData
 */
function ReprocessingHistoricalData(params) {
  AbstractClass.call(this, {'class': 'ReprocessingHistoricalData'});
  
  /**
   * @type {number}
   */
  this.id = params.id || 0;
  /**
   * @type {Date}
   */
  this.startDate = params.startDate;
  /**
   * @type {Date}
   */
  this.endDate = params.endDate;
}

ReprocessingHistoricalData.prototype = Object.create(AbstractClass.prototype);
ReprocessingHistoricalData.prototype.constructor = ReprocessingHistoricalData;

/**
 * It retrieves a real representation of ReprocessingHistoricalData
 * 
 * @returns {Object}
 */
ReprocessingHistoricalData.prototype.rawObject = function() {
  return Object.assign(AbstractClass.prototype.rawObject.call(this), {
    startDate: this.startDate instanceof Date ? Utils.formatDateToTimezone(this.startDate) : this.startDate,
    endDate: this.endDate instanceof Date ? Utils.formatDateToTimezone(this.endDate) : this.endDate
  });
};

/**
 * It retrieves a formatted object to send via TCP
 * 
 * @returns {Object}
 */
ReprocessingHistoricalData.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    start_date: this.startDate instanceof Date ? Utils.formatDateToTimezone(this.startDate) : this.startDate,
    end_date: this.endDate instanceof Date ? Utils.formatDateToTimezone(this.endDate) : this.endDate
  });
};

module.exports = ReprocessingHistoricalData;