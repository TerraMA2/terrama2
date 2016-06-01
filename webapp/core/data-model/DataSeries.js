var BaseClass = require('./AbstractData');


var DataSeries = function(params) {
  BaseClass.call(this, {'class': 'DataSeries'});

  this.id = params.id;
  this.name = params.name;
  this.description = params.description;
  this.data_provider_id = params.data_provider_id;
  this.data_series_semantics_id = params.data_series_semantics_id;

  if (params.data_series_semantics)
    this.data_series_semantics = params.data_series_semantics;
  else if (params['DataSeriesSemantic'])
    this.data_series_semantics = params['DataSeriesSemantic'].get();

  this.semantics = params.semantics;
  this.dataSets = params.dataSets || [];
};

DataSeries.prototype = Object.create(BaseClass.prototype);
DataSeries.prototype.constructor = DataSeries;

DataSeries.prototype.toObject = function() {
  var dataSets = [];
  (this.dataSets || []).forEach(function(element) {
    dataSets.push(element instanceof BaseClass ? element.toObject() : element);
  });
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    name: this.name,
    description: this.description,
    data_provider_id: this.data_provider_id,
    // data_series_semantic_name: this.data_series_semantic_name,
    // semantics: this.semantics,
    semantics: this.data_series_semantics.code,
    datasets: dataSets
  })
};

DataSeries.prototype.rawObject = function() {
  var dSets = [];
  this.dataSets.forEach(function(dSet) {
    if (dSet instanceof BaseClass)
      dSets.push(dSet.toObject());
    else
      dSets.push(dSet);
  })
  return {
    id: this.id,
    name: this.name,
    description: this.description,
    data_provider_id: this.data_provider_id,
    data_series_semantic_id: this.data_series_semantic_id,
    data_series_semantic_code: this.data_series_semantics.code,
    data_series_semantics: this.data_series_semantics,
    dataSets: dSets
  }
}

module.exports = DataSeries;
