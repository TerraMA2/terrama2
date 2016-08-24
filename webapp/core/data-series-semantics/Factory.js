var AbstractSemantics = require("./../AbstractSemantics");
var DcpInpe = require('./DcpInpe');
var DcpPostgis = require('./DcpPostgis');
var WildFire = require('./WildFire');
var OccurrencePostgis = require('./OccurrencePostgis');
var Grads = require('./Grads');
var FileStaticDataOgr = require('./FileStaticDataOgr');
var PostgisStaticDataOgr = require('./PostgisStaticDataOgr');
var AnalysisMonitoredObject = require('./AnalysisMonitoredObject');
var GridGeoTiff = require('./GridGeoTiff');
var DcpToa5 = require('./DcpToa5');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var PluginLoader = require('./../PluginLoader');

// nodejs
var fs = require('fs');
var path = require('path');

function availableTypes() {
  var output = [];

  // native types
  output.push(DcpInpe);
  output.push(DcpPostgis);
  output.push(WildFire);
  output.push(OccurrencePostgis);
  // output.push(AnalysisPostgis);
  output.push(FileStaticDataOgr);
  output.push(AnalysisMonitoredObject);
  output.push(GridGeoTiff);
  output.push(DcpToa5);
  output.push(PostgisStaticDataOgr);
  output.push(Grads);

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


// loading semantics json
var semanticsJsonPath = path.join(__dirname, "../../../share/terrama2/semantics.json");
var semanticsObject = JSON.parse(fs.readFileSync(semanticsJsonPath, 'utf-8'));

// list of all instances of semantics type
var semanticsTypes = [];
var supportedTypes = availableTypes().concat(availablePlugins());
semanticsObject.forEach(function(semantics) {
  supportedTypes.some(function(type, typeIndex, typeArr) {
    if (type.identifier() === semantics.code) {
      semanticsTypes.push(new type(semantics));

      // remove it from list
      typeArr.splice(typeIndex, 1);
      return true;
    }
  })
});

var Factory = module.exports = {};

function getSemanticHelper(identifier) {
  var semanticsOutput = null;
  semanticsTypes.some(function(semantics) {
    if (semantics.get().code === identifier) {
      semanticsOutput = semantics;
      return true;
    }
  });

  if (semanticsOutput)
    return semanticsOutput;

  throw new DataSeriesSemanticsError("DataSeriesSemantics is invalid.");
}

Factory.build = function(args) {
  return getSemanticHelper(args.code);
};

Factory.listAll = function() {
  return semanticsTypes;
};
