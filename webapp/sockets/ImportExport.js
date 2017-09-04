'use strict';

/**
 * Socket responsible for handling import and export terrama2 data
 * @class ImportExport
 * @author Raphael Willian da Costa
 * @property {object} iosocket - Sockets object.
 * @property {Object} DataManager - TerraMA² DataManager module
 * @property {Promise} Promise - bluebird Promises module
 */
var ImportExport = function(io) {
  // Sockets object
  var iosocket = io.sockets;
  // TerraMA2 dependencies
  var DataManager = require("./../core/DataManager");
  var TcpService = require("./../core/facade/tcp-manager/TcpService");
  var ImportProjectMember = require("./../core/ImportProject");
  var ExportProjectMember = require("./../core/ExportProject");

  // bluebird promise
  var Promise = require("bluebird");

  // clone module
  var clone = require('clone');

  // Socket connection event
  iosocket.on('connection', function(client) {

    var countObjectProperties = function(object) {
      var count = 0;
      
      if(object !== undefined && object !== null && typeof object === "object")
        for(var key in object) if(object.hasOwnProperty(key)) count++;

      return count;
    };

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

      ImportProjectMember(json, function(result){
        if (result.tcpOutput){
          TcpService.send(result.tcpOutput);
          delete result.tcpOutput;
        }
        client.emit("importResponse", result);
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

      ExportProjectMember(json, function(result){
        client.emit("exportResponse", result);
      })
    });

    /**
     * TerraMA2 Dependencies List Listener.
     * @param {Object} json - A javascript object containing the object id to list its dependencies.
     */
    client.on("getDependencies", function(json) {
      var output = {};

      var _emitError = function(err) {
        client.emit("getDependenciesResponse", {
          err: err.toString(),
          status: 400
        });
      };

      var isInArray = function(id, array) {
        for(var i = 0, arrayLength = array.length; i < arrayLength; i++) {
          if(array[i] == id) return true;
        }

        return false;
      };

      var getDataSeriesDependencies = function(id, staticDataSeries, outputIndex) {
        var prefix = (staticDataSeries !== null ? ("DataSeries" + (staticDataSeries ? "Static" : "") + "_") : "");

        return DataManager.getDataSeries({id: id}).then(function(dataSeries) {
          if(prefix === "") prefix = "DataSeries" + (dataSeries.data_series_semantics.temporality == "STATIC" ? "Static" : "") + "_";

          if(outputIndex !== null) {
            var type = (dataSeries.data_series_semantics.temporality == "STATIC" ? "DataSeriesStatic" : "DataSeries");
            if(output[outputIndex][type] === undefined) output[outputIndex][type] = [];
            output[outputIndex][type].push(id);
          }

          return DataManager.getDataProvider({id: dataSeries.data_provider_id});
        }).then(function(dataProvider) {
          if(output[prefix + id] === undefined || output[prefix + id].DataProviders === undefined || !isInArray(dataProvider.id, output[prefix + id].DataProviders)) {
            if(output[prefix + id] === undefined) output[prefix + id] = {};
            if(output[prefix + id].DataProviders === undefined) output[prefix + id].DataProviders = [];

            output[prefix + id].DataProviders.push(dataProvider.id);
          }

          return DataManager.getCollector({data_series_output: id});
        }).then(function(collector) {
          var collectorDataSeriesInput = collector.data_series_input;

          var dataSeriesPromises = [
            DataManager.getDataSeries({id: collectorDataSeriesInput}).then(function(dataSeries) {
              if(output[prefix + id] === undefined || output[prefix + id].DataSeries === undefined || !isInArray(dataSeries.id, output[prefix + id].DataSeries)) {
                if(output[prefix + id] === undefined) output[prefix + id] = {};
                if(output[prefix + id].DataSeries === undefined) output[prefix + id].DataSeries = [];

                output[prefix + id].DataSeries.push(dataSeries.id);
                return DataManager.getDataProvider({id: dataSeries.data_provider_id});
              }
            }).then(function(dataProvider) {
              if(dataProvider !== undefined && (output["DataSeries_" + collectorDataSeriesInput] === undefined || output["DataSeries_" + collectorDataSeriesInput].DataProviders === undefined || !isInArray(dataProvider.id, output["DataSeries_" + collectorDataSeriesInput].DataProviders))) {
                if(output["DataSeries_" + collectorDataSeriesInput] === undefined) output["DataSeries_" + collectorDataSeriesInput] = {};
                if(output["DataSeries_" + collectorDataSeriesInput].DataProviders === undefined) output["DataSeries_" + collectorDataSeriesInput].DataProviders = [];

                output["DataSeries_" + collectorDataSeriesInput].DataProviders.push(dataProvider.id);
              }
            })
          ];

          for(var j = 0, intersectionsLength = collector.intersection.length; j < intersectionsLength; j++) {
            if(output[prefix + id] === undefined) output[prefix + id] = {};

            dataSeriesPromises.push(
              getDataSeriesDependencies(collector.intersection[j].dataseries_id, null, prefix + id)
            );
          }

          return Promise.all(dataSeriesPromises).catch(_emitError);
        }).catch(function(err) {
          return Promise.resolve();
        })
      };

      var promises = [];

      if(json.objectType == "DataSeries") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          promises.push(
            getDataSeriesDependencies(json.ids[i], false, null)
          );
        }
      } else if(json.objectType == "DataSeriesStatic") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          promises.push(
            getDataSeriesDependencies(json.ids[i], true, null)
          );
        }
      } else if(json.objectType == "Analysis") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          promises.push(
            DataManager.getAnalysis({id: json.ids[i]}, null, true).then(function(analysis) {
              if(analysis.dataSeries.id !== undefined && (output["Analysis_" + analysis.id] === undefined || output["Analysis_" + analysis.id].DataSeries === undefined || !isInArray(analysis.dataSeries.id, output["Analysis_" + analysis.id].DataSeries))) {
                if(output["Analysis_" + analysis.id] === undefined) output["Analysis_" + analysis.id] = {};

                var analysisDataseriesListPromises = [getDataSeriesDependencies(analysis.dataSeries.id, null, "Analysis_" + analysis.id)];
              } else {
                var analysisDataseriesListPromises = [];
              }

              for(var j = 0, analysisDSLength = analysis.analysis_dataseries_list.length; j < analysisDSLength; j++) {
                if(output["Analysis_" + analysis.id] === undefined) output["Analysis_" + analysis.id] = {};

                analysisDataseriesListPromises.push(
                  getDataSeriesDependencies(analysis.analysis_dataseries_list[j].data_series_id, null, "Analysis_" + analysis.id)
                );
              }

              return Promise.all(analysisDataseriesListPromises).catch(_emitError);
            })
          );
        }
      } else if(json.objectType == "Views") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          promises.push(
            DataManager.getView({id: json.ids[i]}).then(function(view) {
              if(output["Views_" + view.id] === undefined) output["Views_" + view.id] = {};

              return getDataSeriesDependencies(view.dataSeries.id, null, "Views_" + view.id);
            })
          );
        }
      } else if(json.objectType == "Legends") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          output["Legends_" + json.ids[i]] = {};
        }
      } else if(json.objectType == "Alerts") {
        for(var i = 0, idsLength = json.ids.length; i < idsLength; i++) {
          promises.push(
            DataManager.getAlert({id: json.ids[i]}).then(function(alert) {
              if(output["Alerts_" + alert.id] === undefined) {
                output["Alerts_" + alert.id] = {
                  Legends: [alert.legend.id]
                };
              }

              var getDataSeriesDependenciesPromise = getDataSeriesDependencies(alert.data_series_id, null, "Alerts_" + alert.id);

              var listAnalysisPromise = DataManager.listAnalysis({project_id: alert.project_id}).then(function(analysisList) {
                var analysisDataseriesListPromises = [];

                for(var j = 0, analysisLength = analysisList.length; j < analysisLength; j++) {
                  if(analysisList[j].dataSeries.id === alert.data_series_id) {
                    if(output["Alerts_" + alert.id].Analysis === undefined) output["Alerts_" + alert.id].Analysis = [];
                    output["Alerts_" + alert.id].Analysis.push(analysisList[j].id);

                    if(analysisList[j].analysis_dataseries_list.length > 0 && output["Analysis_" + analysisList[j].id] === undefined)
                      output["Analysis_" + analysisList[j].id] = {};

                    for(var x = 0, analysisDSLength = analysisList[j].analysis_dataseries_list.length; x < analysisDSLength; x++)
                      analysisDataseriesListPromises.push(getDataSeriesDependencies(analysisList[j].analysis_dataseries_list[x].data_series_id, null, "Analysis_" + analysisList[j].id));

                    break;
                  }
                }

                return Promise.all(analysisDataseriesListPromises);
              });

              return Promise.join(getDataSeriesDependenciesPromise, listAnalysisPromise);
            })
          );
        }
      }

      Promise.all(promises).then(function() {
        client.emit("getDependenciesResponse", { status: 200, data: output, projectId: json.projectId });
      }).catch(_emitError);
    });
  });
};

module.exports = ImportExport;
