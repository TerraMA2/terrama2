var DataSet = require('./DataSet');
var isString = require("./../Utils").isString;

function DataSetDcp(params) {
  DataSet.call(this, params);

  this.positionWkt = params.positionWkt || null;

  this.setPosition(params.position);
}

DataSetDcp.prototype = Object.create(DataSet.prototype);
DataSetDcp.prototype.constructor = DataSetDcp;

DataSetDcp.prototype.setPosition = function(position) {
  // Try parse if it is string
  if (isString(position)) {
    try {
      this.position = JSON.parse(position);
    } catch (e) {
      // ???
      console.log("Error during position parse. " + e.toString());
      this.position = position;
    }
  } else { // null / object
    this.position = position;
  }
};

DataSetDcp.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {position: this.positionWkt || this.position});
};

DataSetDcp.prototype.rawObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {position: this.position});
};

module.exports = DataSetDcp;
