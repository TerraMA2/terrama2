'use strict';

/**
 * Socket responsible for handling import and export terrama2 data
 * @class ImportExport
 * @author Raphael Willian da Costa
 * @property {object} iosocket - Sockets object.
 * @property {Object} DataManager - TerraMA² DataManager module
 */
var ImportExport = function(io) {
  // Sockets object
  var iosocket = io.sockets;
  // TerraMA2 dependencies
  var TcpService = require("./../core/facade/tcp-manager/TcpService");
  var ImportProjectMember = require("./../core/ImportProject");
  var ExportProjectMember = require("./../core/ExportProject");
  var ObjectDependencies = require("./../core/ObjectDependencies");

  // Socket connection event
  iosocket.on('connection', function(client) {

    /**
     * TerraMA2 Import Listener. It prepares a json and load them to Database if does not exist.
     * @param {Object} json - A javascript object containing what intend to import.
     * Note it must be group by generic name pointing to array of elements. i.e {"Projects": []}
     * @example
     * - Importing a single project
     * client.emit("import", {"Projects": [{"id": 1, "name": "foo", "version": 1}]})
     * - Importing multiple terrama2 datas
     * client.emit("import", {
     *   "Projects":      [projectA, projectB, ...projectN],
     *   "DataProviders": [providerA, providerB, ...providerN],
     *   "DataSeries":    [dataSeriesA, dataSeriesB, ...dataSeriesN],
     *   "Analysis":      [analysisA, analysisB, ...analysisN],
     *   "Views":      [viewA, viewB, ...viewN],
     *   "Legends":      [legendA, legendB, ...legendN],
     *   "Alerts":      [alertA, alertB, ...alertN],
     * })
     */
    client.on("import", function(json) {
      return ImportProjectMember(json).then(function(result){
        if (result.tcpOutput){
          TcpService.send(result.tcpOutput);
          delete result.tcpOutput;
        }
        client.emit("importResponse", result);
      }).catch(function(err){
        client.emit("importResponse", err);
      });
    });

    /**
     * TerraMA2 Export Listener. This function does not save file in disk. It just creates a json to be exported
     * @param {Object} json - A javascript object containing what intend to export.
     * Note it must be group by generic name pointing to array of elements with an identifier parameter. i.e {"Projects": []}
     * @example
     * - Exporting a single project
     * client.emit("export", {"Projects": [{"id": 1}]})
     * - Exporting multiple terrama2 datas
     * client.emit("export", {
     *   "Projects":      [projectA, projectB, ...projectN],
     *   "DataProviders": [providerA, providerB, ...providerN],
     *   "DataSeries":    [dataSeriesA, dataSeriesB, ...dataSeriesN],
     *   "Analysis":      [analysisA, analysisB, ...analysisN],
     * })
     */
    client.on("export", function(json) {
      return ExportProjectMember(json).then(function(result){
        client.emit("exportResponse", result);
      }).catch(function(err){
        client.emit("exportResponse", err);
      });
    });

    /**
     * TerraMA2 Dependencies List Listener.
     * @param {Object} json - A javascript object containing the object id to list its dependencies.
     */
    client.on("getDependencies", function(json) {
      return ObjectDependencies(json).then(function(result){
        client.emit("getDependenciesResponse", result);
      }).catch(function(err){
        client.emit("getDependenciesResponse", err);
      });
    });
  });
};

module.exports = ImportExport;
