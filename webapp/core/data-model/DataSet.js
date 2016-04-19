var AbstractClass = require("./AbstractData");

function DataSet(params) {
  AbstractClass.call(this, {'class': 'DataSet'});
  
  this.id = params.id;
  this.data_series_id = params.data_series_id;
  this.active = params.active || false;
  this.format = params.format || {};
}

DataSet.prototype = Object.create(AbstractClass.prototype);
DataSet.prototype.constructor = DataSet;

DataSet.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    id: this.id,
    data_series_id: this.data_series_id,
    active: this.active
  });
};

module.exports = DataSet;