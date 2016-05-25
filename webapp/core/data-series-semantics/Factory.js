var AbstractSemantics = require("./../AbstractSemantics");
var DcpInpe = require('./DcpInpe');
var DcpPostgis = require('./DcpPostgis');
var WildFire = require('./WildFire');
var OccurrencePostgis = require('./OccurrencePostgis');
var AnalysisPostgis = require('./AnalysisPostgis');
var StaticDataOgr = require('./StaticDataOgr');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var PluginLoader = require('./../PluginLoader');

var Factory = module.exports = {};

function availableTypes() {
  var output = [];

  // native types
  output.push(DcpInpe);
  output.push(DcpPostgis);
  output.push(WildFire);
  output.push(OccurrencePostgis);
  output.push(AnalysisPostgis);
  output.push(StaticDataOgr)

  var plugins = availablePlugins();

  return output.concat(plugins);
}

function availablePlugins() {
  var pluginList = PluginLoader.plugin.plugins;
  var output = [];
  for(var key in pluginList) {
    if (pluginList.hasOwnProperty(key)) {
      var klass = pluginList[key].object;

      if (klass && klass.prototype instanceof AbstractSemantics) {
        output.push(klass);
      }
    }
  }

  return output;
}

function getSemanticHelper(identifier) {
  var types = availableTypes();

  var dataSeriesSemantics;

  types.forEach(function(semantics) {
    if (semantics.identifier() === identifier) {
      dataSeriesSemantics = semantics;
      return;
    }
  });

  if (dataSeriesSemantics)
    return dataSeriesSemantics;

  throw new DataSeriesSemanticsError("DataSeriesSemantics is invalid.");
}

Factory.build = function(args) {
  var dataSeriesSemantics = getSemanticHelper(args.name);
  return new dataSeriesSemantics();
};

Factory.getDataSeriesSemantics = function(identifier) {
  var dataSeriesSemantics = getSemanticHelper(identifier);
  return {
    name: dataSeriesSemantics.identifier(),
    form: dataSeriesSemantics.form(),
    schema: dataSeriesSemantics.schema(),
    demand: dataSeriesSemantics.demand()
  };
};

Factory.listAll = function() {
  var output = [];

  var types = availableTypes();

  types.forEach(function(typ) {
    output.push({
      name: typ.identifier(),
      form: typ.form(),
      schema: typ.schema(),
      demand: typ.demand()
    })
  })

  return output;
};
