var AbstractSemantics = require("./../AbstractSemantics");
var DcpInpe = require('./DcpInpe');
var DcpPostgis = require('./DcpPostgis');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var PluginLoader = require('./../PluginLoader');

var Factory = module.exports = {};

function availableTypes() {
  var output = [];

  output.push(DcpInpe);
  output.push(DcpPostgis);

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
    schema: dataSeriesSemantics.schema()
  };
};

Factory.listAll = function() {
  var output = [];

  // adding DcpInpe
  output.push({
    name: DcpInpe.identifier(),
    form: DcpInpe.form(),
    schema: DcpInpe.schema()
  });

  // adding DcpPostgis
  output.push({
    name: DcpPostgis.identifier(),
    form: DcpPostgis.form(),
    schema: DcpPostgis.schema()
  });

  // checking for available plugins
  var plugins = availablePlugins();

  // todo: validation for exclude inconsistent plugins
  plugins.forEach(function(plugin) {
    output.push({
      name: plugin.identifier(),
      form: plugin.form(),
      schema: plugin.schema()
    })
  });

  return output;
};