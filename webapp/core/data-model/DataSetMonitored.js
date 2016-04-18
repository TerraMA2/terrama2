var DataSet = require('./DataSet');


function DataSetMonitored(params) {
  DataSet.call(this, params);
}

DataSetMonitored.prototype = Object.create(DataSet.prototype);
DataSetMonitored.prototype.constructor = DataSetMonitored;


DataSetMonitored.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.call(this), {});
};

module.exports = DataSetMonitored;