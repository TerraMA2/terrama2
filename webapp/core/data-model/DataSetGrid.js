var DataSet = require('./DataSet');


function DataSetGrid(params) {
  DataSet.call(this, params);
}

DataSetGrid.prototype = Object.create(DataSet.prototype);
DataSetGrid.prototype.constructor = DataSetGrid;


DataSetGrid.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {});
};

module.exports = DataSetGrid;
