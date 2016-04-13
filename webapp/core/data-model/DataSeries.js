var BaseClass = require('./AbstractData');


var DataSeries = function(params) {
  BaseClass.call(this, {'class': 'DataSeries'});
  
  this.id = params.id;
  this.name = params.name;
  this.description = params.description;
  this.data_provider_id = params.data_provider_id;
  this.data_series_semantic_name = params.data_series_semantic_name;
  this.semantics = params.semantics;
  this.datasets = params.dataSets || [];
};

DataSeries.prototype = Object.create(BaseClass.prototype);
DataSeries.prototype.constructor = DataSeries;

DataSeries.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    name: this.name,
    description: this.description,
    data_provider_id: this.data_provider_id,
    data_series_semantic_name: this.data_series_semantic_name,
    semantics: this.semantics
  })
};

module.exports = DataSeries;