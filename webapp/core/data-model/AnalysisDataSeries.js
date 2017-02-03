var BaseClass = require("./AbstractData");
var Utils = require('./../Utils');

var AnalysisDataSeries = module.exports = function(params) {
  BaseClass.call(this, {'class': "AnalysisDataSeries"});

  this.analysis_id = params.analysis_id;

  this.id = params.id;
  this.data_series_id = params.data_series_id;

  if (params.AnalysisDataSeriesType)
    this.type = params.AnalysisDataSeriesType.get();
  else
    this.type = params.type || {};

  if (params.AnalysisDataSeriesMetadata)
    this.setMetadata(params.AnalysisDataSeriesMetadata);
  else
    this.metadata = params.metadata || {};

  this.type_id = params.type_id;
  this.alias = params.alias;

  this.dataSeries = {};
};

AnalysisDataSeries.prototype = Object.create(BaseClass.prototype);
AnalysisDataSeries.prototype.constructor = AnalysisDataSeries;

AnalysisDataSeries.prototype.setMetadata = function(metadataObject) {
  if (metadataObject instanceof Array)
    this.metadata = Utils.formatMetadataFromDB(metadataObject);
  else
    this.metadata = metadataObject;
};

AnalysisDataSeries.prototype.setDataSeries = function(dataSeries) {
  this.dataSeries = dataSeries;
};

AnalysisDataSeries.prototype.toObject = function() {
  return Object.assign(BaseClass.prototype.toObject.call(this), {
    id: this.id,
    data_series_id: this.data_series_id,
    type: this['type_id'],
    alias: this.alias,
    metadata: this.metadata,
    analysis_id: this.analysis_id
  });
};

AnalysisDataSeries.prototype.rawObject = function() {
  var obj = this.toObject();

  obj.dataSeries = this.dataSeries instanceof BaseClass ? this.dataSeries.rawObject() : this.dataSeries;
  return obj;
};