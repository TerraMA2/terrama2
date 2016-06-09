var DataSet = require('./DataSet');


function DataSetMonitored(params) {
  DataSet.call(this, params);
  this.geometry_column = params.geometry_column;
  this.time_column = params.time_column;
  this.id_column = params.id_column;
  this.srid = params.srid;
}

DataSetMonitored.prototype = Object.create(DataSet.prototype);
DataSetMonitored.prototype.constructor = DataSetMonitored;


DataSetMonitored.prototype.toObject = function() {
  return Object.assign(DataSet.prototype.toObject.call(this), {
    geometry_column: this.geometry_column,
    time_column: this.time_column,
    srid: this.srid,
    id_column: this.id_column
  });
};

module.exports = DataSetMonitored;