var BaseClass = require("./AbstractData");

var AnalysisDataSeries = module.exports = function(params) {
  BaseClass.call(this, {'class': "AnalysisDataSeries"});

  this.id = params.id;
  this.data_series_id = params.data_series_id;

  this.type_id = params.type_id;
  this.alias = params.alias;

  this.metadata = params.metadata || {};
};

AnalysisDataSeries.prototype = Object.create(BaseClass.prototype);
AnalysisDataSeries.prototype.constructor = AnalysisDataSeries;

AnalysisDataSeries.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    data_series_id: this.data_series_id,
    type: this['type_id'],
    alias: this.alias,
    metadata: this.metadata
  });
};