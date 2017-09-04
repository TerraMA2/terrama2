'use strict';

/**
 * Socket responsible for handling import and export terrama2 data
 * @class ImportExport
 * @author Raphael Willian da Costa
 * @property {object} iosocket - Sockets object.
 * @property {Object} Utils - TerraMA² Utils module
 * @property {Enums} Enums - TerraMA² Enums
 * @property {Object} DataManager - TerraMA² DataManager module
 * @property {Promise} Promise - bluebird Promises module
 */
var ImportExport = function(io) {
  // Sockets object
  var iosocket = io.sockets;
  // TerraMA2 dependencies
  var Utils = require('./../core/Utils');
  var Enums = require('./../core/Enums');
  var DataManager = require("./../core/DataManager");
  var TcpService = require("./../core/facade/tcp-manager/TcpService");
  var ImportProjectMember = require("./../core/ImportProject");

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
      var output = {
        Projects: [],
        DataProviders: [],
        DataSeries: [],
        Collectors: [],
        Analysis: [],
        Views: [],
        Alerts: [],
        Legends: []
      };

      var _emitError = function(err) {
        client.emit("exportResponse", {
          err: err.toString(),
          status: 400
        });
      };

      var target;

      // it adds a $id in object
      var _addID = function(object) {
        object.$id = object.id;
        delete object.id;
        return object;
      };

      var addID = function(object) {
        var output = object.rawObject();
        return _addID(output);
      };

      var isInArray = function(id, array) {
        for(var i = 0, arrayLength = array.length; i < arrayLength; i++) {
          if(array[i].id == id || array[i].$id == id) return true;
        }

        return false;
      };

      var getDataSeries = function(id) {
        return DataManager.getDataSeries({id: id}).then(function(dataSeries) {
          if(!isInArray(dataSeries.id, output.DataSeries)) {
            dataSeries.project_id = null;
            output.DataSeries.push(addID(dataSeries));

            return DataManager.getDataProvider({id: dataSeries.data_provider_id});
          } else {
            return Promise.resolve();
          }
        }).then(function(dataProvider) {
          if(dataProvider !== undefined && !isInArray(dataProvider.id, output.DataProviders)) {
            dataProvider.project_id = null;
            output.DataProviders.push(addID(dataProvider));
          }

          return DataManager.getCollector({data_series_output: id});
        }).then(function(collector) {
          if(!isInArray(collector.id, output.Collectors)) {
            collector.service_instance_id = null;

            var collectorToAdd = addID(collector);

            if(countObjectProperties(collectorToAdd.schedule) > 0)
              collectorToAdd.schedule.scheduleType = collectorToAdd.schedule_type;

            output.Collectors.push(collectorToAdd);

            var dataSeriesPromises = [
              DataManager.getDataSeries({id: collector.data_series_input}).then(function(dataSeries) {
                if(!isInArray(dataSeries.id, output.DataSeries)) {
                  dataSeries.project_id = null;
                  output.DataSeries.push(addID(dataSeries));
                  return DataManager.getDataProvider({id: dataSeries.data_provider_id});
                }
              }).then(function(dataProvider) {
                if(dataProvider !== undefined && !isInArray(dataProvider.id, output.DataProviders)) {
                  dataProvider.project_id = null;
                  output.DataProviders.push(addID(dataProvider));
                }
              })
            ];

            for(var j = 0, intersectionsLength = collector.intersection.length; j < intersectionsLength; j++) {
              dataSeriesPromises.push(
                getDataSeries(collector.intersection[j].dataseries_id)
              );
            }

            return Promise.all(dataSeriesPromises).catch(_emitError);
          } else {
            return Promise.resolve();
          }
        }).catch(function(err) {
          return Promise.resolve();
        })
      };

      var promises = [];
      if(json.Projects) {
        target = json.Projects[0] || {};

        promises.push(DataManager.getProject({id: target.id}).then(function(project) {
          var projectId = project.id;
          project.$id = project.id;
          delete project.id;

          output.Projects.push(project);

          var providers = DataManager.listDataProviders({project_id: projectId});
          providers.forEach(function(provider) {
            output.DataProviders.push(addID(provider));
          });

          DataManager.listDataSeries({dataProvider: { project_id: projectId }}).then(function(dataSeriesList) {
            dataSeriesList.forEach(function(dataSeries) {
              dataSeries.data_series_semantics_code = dataSeries.data_series_semantics.code;
              var dSeries = addID(dataSeries)
              delete dSeries.data_series_semantics;
              delete dSeries.data_series_id;
              output.DataSeries.push(dSeries);
            });
          });
        }).catch(_emitError));

        promises.push(DataManager.listCollectors({DataProvider: {project_id: target.id}}).then(function(collectors) {
          collectors.forEach(function(collector) {
            var collectorToAdd = addID(collector);

            if(countObjectProperties(collectorToAdd.schedule) > 0)
              collectorToAdd.schedule.scheduleType = collectorToAdd.schedule_type;

            output.Collectors.push(collectorToAdd);
          });
        }));

        promises.push(DataManager.listAnalysis({project_id: target.id}).then(function(analysisList) {
          analysisList.forEach(function(analysis) {
            var rawAnalysis = analysis.rawObject();
            rawAnalysis.$id = rawAnalysis.id;
            delete rawAnalysis.id;

            rawAnalysis.analysis_dataseries_list.forEach(function(analysisDS) {
              analysisDS.$id = analysisDS.id;
              delete analysisDS.id;
            });

            if(countObjectProperties(rawAnalysis.schedule) > 0)
              rawAnalysis.schedule.scheduleType = rawAnalysis.schedule_type;

            output.Analysis.push(rawAnalysis);
          });
        }));
        // do not include the alert views (source_type = 4)
        promises.push(DataManager.listViews({project_id: target.id, source_type: {$ne: 4}}).then(function(viewsList) {
          viewsList.forEach(function(view) {
            var viewToAdd = addID(view);

            if(countObjectProperties(viewToAdd.schedule) > 0)
              viewToAdd.schedule.scheduleType = viewToAdd.schedule_type;

            output.Views.push(viewToAdd);
          });
        }));

        promises.push(DataManager.listAlerts({project_id: target.id}).then(function(alertsList) {
          alertsList.forEach(function(alert) {
            var alertToAdd = addID(alert);
            var legend = alertToAdd.legend;

            if(countObjectProperties(alertToAdd.schedule) > 0)
              alertToAdd.schedule.scheduleType = alertToAdd.schedule_type;
              
            alertToAdd.legend_id = legend.id;
            delete alertToAdd.legend;

            output.Alerts.push(alertToAdd);
          });
        }));

        promises.push(DataManager.listLegends({project_id: target.id}).then(function(legends) {
          legends.forEach(function(legend) {
            output.Legends.push(addID(legend));
          });
        }));
      } // end if projects

      if(json.DataProviders) {
        for(var i = 0, dataProvidersLength = json.DataProviders.length; i < dataProvidersLength; i++) {
          promises.push(
            DataManager.getDataProvider({id: json.DataProviders[i].id}).then(function(dataProvider) {
              if(!isInArray(dataProvider.id, output.DataProviders)) {
                dataProvider.project_id = null;
                output.DataProviders.push(addID(dataProvider));
              }
            })
          );
        }
      }

      if(json.DataSeries) {
        for(var i = 0, dataSeriesLength = json.DataSeries.length; i < dataSeriesLength; i++) {
          promises.push(
            getDataSeries(json.DataSeries[i].id)
          );
        }
      }

      if(json.Collectors) {
        for(var i = 0, collectorsLength = json.Collectors.length; i < collectorsLength; i++) {
          promises.push(
            DataManager.getCollector({id: json.Collectors[i].id}).then(function(collector) {
              if(!isInArray(collector.id, output.Collectors)) {
                collector.service_instance_id = null;

                var collectorToAdd = addID(collector);

                if(countObjectProperties(collectorToAdd.schedule) > 0)
                  collectorToAdd.schedule.scheduleType = collectorToAdd.schedule_type;

                output.Collectors.push(collectorToAdd);

                var dataSeriesPromises = [
                  getDataSeries(collector.data_series_input),
                  getDataSeries(collector.data_series_output)
                ];

                for(var j = 0, intersectionsLength = collector.intersection.length; j < intersectionsLength; j++) {
                  dataSeriesPromises.push(
                    getDataSeries(collector.intersection[j].dataseries_id)
                  );
                }

                return Promise.all(dataSeriesPromises).catch(_emitError);
              } else {
                return Promise.resolve();
              }
            })
          );
        }
      }

      if(json.Analysis) {
        for(var i = 0, analysisLength = json.Analysis.length; i < analysisLength; i++) {
          promises.push(
            DataManager.getAnalysis({id: json.Analysis[i].id}, null, true).then(function(analysis) {
              if(!isInArray(analysis.id, output.Analysis)) {
                var rawAnalysis = analysis.rawObject();
                rawAnalysis.$id = rawAnalysis.id;
                delete rawAnalysis.id;
                rawAnalysis.project_id = null;
                rawAnalysis.service_instance_id = null;

                if(countObjectProperties(rawAnalysis.schedule) > 0)
                  rawAnalysis.schedule.scheduleType = rawAnalysis.schedule_type;

                output.Analysis.push(rawAnalysis);

                var analysisDataseriesListPromises = rawAnalysis.dataSeries.id !== undefined ? [getDataSeries(rawAnalysis.dataSeries.id)] : [];

                for(var j = 0, analysisDSLength = rawAnalysis.analysis_dataseries_list.length; j < analysisDSLength; j++) {
                  rawAnalysis.analysis_dataseries_list[j].$id = rawAnalysis.analysis_dataseries_list[j].id;
                  delete rawAnalysis.analysis_dataseries_list[j].id;

                  analysisDataseriesListPromises.push(
                    getDataSeries(rawAnalysis.analysis_dataseries_list[j].data_series_id)
                  );
                }

                return Promise.all(analysisDataseriesListPromises).catch(_emitError);
              } else {
                return Promise.resolve();
              }
            })
          );
        }
      }

      if(json.Views) {
        for(var i = 0, viewsLength = json.Views.length; i < viewsLength; i++) {
          promises.push(
            DataManager.getView({id: json.Views[i].id}).then(function(view) {
              if(!isInArray(view.id, output.Views)) {
                view.projectId = null;
                view.serviceInstanceId = null;

                var viewToAdd = addID(view);

                if(countObjectProperties(viewToAdd.schedule) > 0)
                  viewToAdd.schedule.scheduleType = viewToAdd.schedule_type;

                output.Views.push(viewToAdd);

                return getDataSeries(view.dataSeries.id);
              } else {
                return Promise.resolve();
              }
            })
          );
        }
      }

      if(json.Legends) {
        for(var i = 0, legendsLength = json.Legends.length; i < legendsLength; i++) {
          promises.push(
            DataManager.getLegend({id: json.Legends[i].id}).then(function(legend) {
              if(!isInArray(legend.id, output.Legends)) {
                legend.project_id = null;
                output.Legends.push(addID(legend));
              }
            })
          );
        }
      }

      if(json.Alerts) {
        for(var i = 0, alertsLength = json.Alerts.length; i < alertsLength; i++) {
          promises.push(
            DataManager.getAlert({id: json.Alerts[i].id}).then(function(alert) {
              if(!isInArray(alert.id, output.Alerts)) {
                alert.project_id = null;
                alert.service_instance_id = null;

                var alertToAdd = addID(alert);
                var legend = alertToAdd.legend;

                alertToAdd.automatic_schedule.scheduleType = 4;
                alertToAdd.legend_id = legend.id;
                delete alertToAdd.legend;

                output.Alerts.push(alertToAdd);

                return getDataSeries(alert.data_series_id);
              } else {
                return Promise.resolve();
              }
            })
          );
        }
      }

      Promise.all(promises).then(function() {
        if(output.Projects.length === 0) delete output.Projects;
        if(output.DataProviders.length === 0) delete output.DataProviders;
        if(output.DataSeries.length === 0) delete output.DataSeries;
        if(output.Collectors.length === 0) delete output.Collectors;
        if(output.Analysis.length === 0) delete output.Analysis;
        if(output.Views.length === 0) delete output.Views;
        if(output.Alerts.length === 0) delete output.Alerts;
        if(output.Legends.length === 0) delete output.Legends;

        client.emit("exportResponse", { status: 200, data: output, projectName: json.currentProjectName, fileName: json.fileName });
      }).catch(_emitError);
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
