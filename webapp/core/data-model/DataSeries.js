'use strict';

// dependencies
var BaseClass = require('./AbstractData');
var DataSetFactory = require('./DataSetFactory');
var DataProvider = require("./DataProvider");

/**
 * A TerraMA² DataSeries representation
 */
var DataSeries = function(params) {
  BaseClass.call(this, {'class': 'DataSeries'});

  this.id = params.id;
  this.name = params.name;
  this.description = params.description;
  this.data_provider_id = params.data_provider_id;
  this.data_series_semantics_id = params.data_series_semantics_id;
  this.active = params.active;

  if (params.DataProvider) {
    this.setDataProvider(params.DataProvider);
  } else {
    this.setDataProvider(params.dataProvider || {});
  }

  if (params.data_series_semantics) {
    this.data_series_semantics = params.data_series_semantics;
  } else if (params.DataSeriesSemantic) {
    this.data_series_semantics = params.DataSeriesSemantic.get();
  }

  this.semantics = params.semantics;

  if (params.DataSets) {
    this.setDataSets(params.DataSets);
  } else {
    this.dataSets = params.dataSets || [];
  }
};

// javascript inherits model
DataSeries.prototype = Object.create(BaseClass.prototype);
DataSeries.prototype.constructor = DataSeries;

/**
 * It sets DataProvider to the property. The dataprovider may be a ORM instance or DataProvider object
 * @param {DataProvider|Object} dataProvider - A data provider values
 */
DataSeries.prototype.setDataProvider = function(dataProvider) {
  var provider = {};
  if (dataProvider instanceof BaseClass) {
    provider = dataProvider;
  } else {
    provider = new DataProvider(dataProvider || provider);
  }

  this.dataProvider = provider;
};

/**
 * It sets DataSet to the property.
 * @param {Array<DataSet>|Array<Object>} dataSets - a data set array with values
 */
DataSeries.prototype.setDataSets = function(dataSets) {
  var output = [];
  dataSets.forEach(function(dataSet) {
    if (dataSet instanceof BaseClass) {
      output.push(dataSet);
    } else { // sequelize instance
      var ob = dataSet.get();
      if (ob.DataSetDcp) {
        Object.assign(ob, ob.DataSetDcp.get());
      } else if (ob.DataSetOccurrence) {
        ob = Object.assign({}, ob.DataSetOccurrence.get(), ob);
      } else if (ob.DataSetMonitored) {
        ob = Object.assign({}, ob.DataSetMonitored.get(), ob);
      }
      output.push(DataSetFactory.build(ob));
    }
  });

  this.dataSets = output;
};

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
    active: this.active,
    datasets: dataSets
  });
};

DataSeries.prototype.rawObject = function() {
  var dSets = [];
  this.dataSets.forEach(function(dSet) {
    if (dSet instanceof BaseClass) {
      dSets.push(dSet.rawObject());
    } else {
      dSets.push(dSet);
    }
  });
  return {
    id: this.id,
    name: this.name,
    description: this.description,
    data_provider_id: this.data_provider_id,
    data_series_semantics_id: this.data_series_semantics_id,
    data_series_semantics_code: this.data_series_semantics.code,
    data_series_semantics: this.data_series_semantics,
    active: this.active,
    dataSets: dSets
  };
};

module.exports = DataSeries;
