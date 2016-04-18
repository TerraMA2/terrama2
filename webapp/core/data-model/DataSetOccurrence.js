var DataSet = require('./DataSet');


function DataSetOccurrence(params) {
  DataSet.call(this, params);
}

DataSetOccurrence.prototype = Object.create(DataSet.prototype);
DataSetOccurrence.prototype.constructor = DataSetOccurrence;


DataSetOccurrence.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {});
};

module.exports = DataSetOccurrence;