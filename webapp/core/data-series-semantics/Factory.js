var AbstractSemantics = require("./../AbstractSemantics");
var DcpInpe = require('./DcpInpe');
var DcpPostgis = require('./DcpPostgis');
var Occurrence = require('./Occurrence');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var PluginLoader = require('./../PluginLoader');

var Factory = module.exports = {};

Factory.build = function(args) {
  switch (args.name) {
    case "DCP-INPE":
      return new DcpInpe();
      break;
    case "DCP-POSTGIS":
      return new DcpPostgis();
      break;
    case "OCCURRENCE":
      return new Occurrence();
    default:
      var pluginList = PluginLoader.plugin.plugins;
      for(var key in pluginList) {
        if (pluginList.hasOwnProperty(key)) {
          var klass = pluginList[key].object;

          if (klass && klass.prototype instanceof AbstractSemantics) {
            if (klass.identifier() === args.name)
              return new klass();
          }
        }
      }

      throw new DataSeriesSemanticsError("DataSeriesSemantics is invalid.");
  }
};