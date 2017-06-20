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
      /**
       * @type {Array<Promise>}
       */
      var promises = [];
      var output = {};
      if(!json || !Utils.isObject(json)) {
        client.emit("importResponse", {
          err: "Unknown error: the parameter must be a object",
          status: 400
        });
        return;
      }

      var _updateID = function(old, object) {
        var o = object.rawObject();
        return Object.assign({$id: old.$id}, o);
      };

      DataManager.orm.transaction(function(t) {
        /**
         * @type {Object} options
         */
        var options = {
          transaction: t
        };
        
        var dataSeriesSemantics;

        promises.push(DataManager.listDataSeriesSemantics({}, options).then(function(semanticsList){
          dataSeriesSemantics = semanticsList;
        }));

        var thereAreProjects = (json.Projects !== undefined && json.Projects.length > 0); 

        // if there any project to import
        if(json.Projects) {
          var projects = json.Projects || [];
          output.Projects = [];
          projects.forEach(function(project) {
            // Try to get project by unique name
            promises.push(DataManager.addProject(project, options).then(function(proj) {
              output.Projects.push(Object.assign({ $id: project.$id }, proj));
            }));
          });
        }

        /**
         * Error helper for promises exception handler. It emits a client response with error
         * @param {Error} err - An exception occurred
         */
        var _emitError = function(err) {
          // TODO: rollback
          return Promise.reject(err);
        };

        return Promise.all(promises).then(function() {
          promises = [];

          var tcpOutput = {};

          if(json.DataProviders) {
            var dataProviders = json.DataProviders || [];
            output.DataProviders = [];
            dataProviders.forEach(function(dataProvider) {
              dataProvider.data_provider_type_id = dataProvider.data_provider_type.id;
              dataProvider.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: dataProvider.project_id}).id : json.selectedProject;

              promises.push(DataManager.addDataProvider(dataProvider, options).then(function(dProvider) {
                if(tcpOutput.DataProviders === undefined) tcpOutput.DataProviders = [];
                tcpOutput.DataProviders.push(dProvider.toObject());

                output.DataProviders.push(_updateID(dataProvider, dProvider));
                return Promise.resolve();
              }));
            });
          }

          return Promise.all(promises).then(function() {
            promises = [];
            if(json.DataSeries) {
              var dataSeries = json.DataSeries ||  [];
              output.DataSeries = [];

              /**
               * Helper for matching id with $id
               * @param {DataSeries} dSeries - A data series from json
               * @param {DataSeries} dSeriesResult - A data series retrieved from database
               */
              var _processDataSeriesAndDataSets = function(dSeries, dSeriesResult) {
                var oldDataSets = dSeries.dataSets;
                dSeriesResult.dataSets.forEach(function(dataSet, index) {
                  dataSet.$id = oldDataSets[index].$id;
                });
                dSeriesResult.$id = dSeries.$id;
                output.DataSeries.push(dSeriesResult);
              };

              dataSeries.forEach(function(dSeries) {
                // preparing to insert in DataBase
                var semantic = dataSeriesSemantics.find(function(dSeriesSemantics){
                  return dSeries.data_series_semantics_code == dSeriesSemantics.code;
                });

                dSeries.data_series_semantics_id = semantic.id;
                dSeries.data_provider_id = Utils.find(output.DataProviders, {$id: dSeries.data_provider_id}).id;
                dSeries.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: dSeries.project_id}).id : json.selectedProject;
                dSeries.dataSets.forEach(function(dSet) {
                  dSet.$id = dSet.id;
                  delete dSet.id;
                });
                // find or create DataSeries
                promises.push(DataManager.addDataSeries(dSeries, null, options).then(function(dSeriesResult) {
                  if(tcpOutput.DataSeries === undefined) tcpOutput.DataSeries = [];
                  tcpOutput.DataSeries.push(dSeriesResult.toObject());

                  // call helper to add IDs in output.DataSeries
                  _processDataSeriesAndDataSets(dSeries, dSeriesResult);
                }));
              });
            }
            return Promise.all(promises).then(function() {
              promises = [];
              if(json.Collectors) {
                var collectors = json.Collectors || [];

                collectors.forEach(function(collector) {
                  collector.data_series_input = Utils.find(output.DataSeries, {$id: collector.input_data_series}).id;

                  var dsOutput = Utils.find(output.DataSeries, {$id: collector.output_data_series});
                  collector.data_series_output = dsOutput.id;

                  var date = {};

                  if(collector.filter && collector.filter.discard_after)
                    date.afterDate = Utils.dateFromFormat(collector.filter.discard_after);

                  if(collector.filter && collector.filter.discard_before)
                    date.beforeDate = Utils.dateFromFormat(collector.filter.discard_before);

                  if(collector.filter && collector.filter.data_series_id)
                    collector.filter.data_series_id = Utils.find(output.DataSeries, {$id: collector.filter.data_series_id}).id;

                  collector.filter.date = date;

                  if(collector.service_instance_id === null) collector.service_instance_id = json.servicesCollect;

                  if(countObjectProperties(collector.schedule) > 0) {
                    delete collector.schedule.id;

                    promises.push(DataManager.addSchedule(collector.schedule, options).then(function(scheduleResult) {
                      collector.schedule_id = scheduleResult.id;

                      return DataManager.addCollector(collector, collector.filter, options).then(function(collectorResult) {
                        collectorResult.project_id = Utils.find(output.DataSeries, {id: collectorResult.data_series_input}).dataProvider.project_id;

                        if(tcpOutput.Collectors === undefined) tcpOutput.Collectors = [];
                        tcpOutput.Collectors.push(collectorResult.toObject());
                      });
                    }));
                  } else {
                    promises.push(DataManager.addCollector(collector, collector.filter, options).then(function(collectorResult) {
                      collectorResult.project_id = Utils.find(output.DataSeries, {id: collectorResult.data_series_input}).dataProvider.project_id;

                      if(tcpOutput.Collectors === undefined) tcpOutput.Collectors = [];
                      tcpOutput.Collectors.push(collectorResult.toObject());
                    }));
                  }
                });
              }

              return Promise.all(promises).then(function() {
                promises = [];

                if(json.Analysis) {
                  var analysisList = json.Analysis || [];
                  analysisList.forEach(function(analysis) {
                    analysis.analysisDataSeries = analysis.analysis_dataseries_list;

                    for(var i = 0; i < analysis.analysisDataSeries.length; ++i) {
                      var ds = analysis.analysisDataSeries[i];
                      for(var k = 0; k < output.DataSeries.length; ++k) {
                        var anDs = output.DataSeries[k];
                        if(ds.data_series_id === anDs.$id) {
                          ds.type_id = ds.type;
                          ds.data_series_id = anDs.id;
                          break;
                        }
                      }
                    }

                    analysis.type_id = analysis.type.id;

                    analysis.instance_id = analysis.service_instance_id;
                    analysis.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: analysis.project_id}).id : json.selectedProject;
                    analysis.script_language_id = analysis.script_language;
                    analysis.grid = analysis.output_grid;
                    analysis.historical = analysis.reprocessing_historical_data;
                    var dataSeriesOutput = Utils.find(output.DataSeries, {
                      $id: analysis.output_dataseries_id
                    });
                    // if there grid analysis, check if there data series id, like resolution data series id.
                    // it must be changed, since it should be a different id
                    if(analysis.grid && analysis.grid.analysis_id) {
                      // TODO: It must retrieve all data series, instead retrieve one per once
                      if(analysis.grid.resolution_data_series_id)
                        analysis.grid.resolution_data_series_id = Utils.find(output.DataSeries, {$id: analysis.grid.resolution_data_series_id}).id;

                      if(analysis.grid.area_of_interest_data_series_id)
                        analysis.grid.area_of_interest_data_series_id = Utils.find(output.DataSeries, {$id: analysis.grid.area_of_interest_data_series_id}).id;
                    }

                    if(dataSeriesOutput.data_series_semantics.data_series_type_name === Enums.DataSeriesType.DCP) {
                      // TODO:
                      console.log("TODO: Analysis DCP export");
                    } else {
                      analysis.dataset_output = dataSeriesOutput.dataSets[0].id;
                    }

                    if(analysis.service_instance_id === null) analysis.service_instance_id = json.servicesAnalysis;
                    if(analysis.instance_id === null) analysis.instance_id = json.servicesAnalysis;

                    if(countObjectProperties(analysis.schedule) || analysis.automatic_schedule.id) {
                      delete analysis.schedule.id;
                      delete analysis.automatic_schedule.id;
                      var scheduleObject;
                      if (analysis.schedule_type == Enums.ScheduleType.AUTOMATIC){
                        scheduleObject = analysis.automatic_schedule;
                        scheduleObject.scheduleType = Enums.ScheduleType.AUTOMATIC;
                      } else {
                        scheduleObject = analysis.schedule;
                      }

                      promises.push(DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
                        if (analysis.schedule_type == Enums.ScheduleType.AUTOMATIC)
                          analysis.automatic_schedule_id = schedule.id;
                        else 
                          analysis.schedule_id = schedule.id;

                        return DataManager.addAnalysis(analysis, options).then(function(analysisResult) {
                          if(tcpOutput.Analysis === undefined) tcpOutput.Analysis = [];
                          tcpOutput.Analysis.push(analysisResult.toObject());
                        });
                      }));
                    } else {
                      promises.push(DataManager.addAnalysis(analysis, options).then(function(analysisResult) {
                        if(tcpOutput.Analysis === undefined) tcpOutput.Analysis = [];
                        tcpOutput.Analysis.push(analysisResult.toObject());
                      }));
                    }
                  });
                }

                return Promise.all(promises).then(function() {
                  promises = [];

                  if(json.Views) {
                    var viewsList = json.Views || [];
                    viewsList.forEach(function(view) {
                      view.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: view.project_id}).id : json.selectedProject;
                      view.data_series_id = Utils.find(output.DataSeries, {$id: view.data_series_id}).id;
                      if(view.service_instance_id === null) view.service_instance_id = json.servicesView;

                      if(countObjectProperties(view.schedule) > 0 || view.automatic_schedule.id) {
                        delete view.schedule.id;
                        delete view.automatic_schedule.id;
                        var scheduleObject;
                        if (view.schedule_type == Enums.ScheduleType.AUTOMATIC){
                          scheduleObject = view.automatic_schedule;
                          scheduleObject.scheduleType = Enums.ScheduleType.AUTOMATIC;
                        } else {
                          scheduleObject = view.schedule;
                        }

                        promises.push(DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
                          if (schedule){
                            if (view.schedule_type == Enums.ScheduleType.AUTOMATIC)
                              view.automatic_schedule_id = schedule.id;
                             else 
                              view.schedule_id = schedule.id;
                          }

                          return DataManager.addView(view, options).then(function(viewResult) {
                            if(tcpOutput.Views === undefined) tcpOutput.Views = [];
                            tcpOutput.Views.push(viewResult.toObject());
                          });
                        }));
                      } else {
                        promises.push(DataManager.addView(view, options).then(function(viewResult) {
                          if(tcpOutput.Views === undefined) tcpOutput.Views = [];
                          tcpOutput.Views.push(viewResult.toObject());
                        }));
                      }
                    });
                  }

                  return Promise.all(promises).then(function() {
                    promises = [];

                    if(json.Legends) {
                      var legendsList = json.Legends || [];
                      output.Legends = [];

                      legendsList.forEach(function(legend) {
                        legend.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: legend.project_id}).id : json.selectedProject;

                        for(var i = 0, levelsLength = legend.levels.length; i < levelsLength; i++)
                          delete legend.levels[i].id;

                        promises.push(
                          DataManager.addLegend(legend, options).then(function(legendResult) {
                            if(tcpOutput.Legends === undefined) tcpOutput.Legends = [];
                              tcpOutput.Legends.push(legendResult);

                            output.Legends.push(_updateID(legend, legendResult));
                          })
                        );
                      });
                    }

                    return Promise.all(promises).then(function() {
                      promises = [];

                      if(json.Alerts) {
                        var alertsList = json.Alerts || [];

                        alertsList.forEach(function(alert) {
                          alert.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: alert.project_id}).id : json.selectedProject;
                          alert.data_series_id = Utils.find(output.DataSeries, {$id: alert.data_series_id}).id;
                          alert.legend_id = Utils.find(output.Legends, {$id: alert.legend_id}).id;

                          if(alert.service_instance_id === null) alert.service_instance_id = json.servicesAlert;

                          delete alert.report_metadata.id;

                          for(var i = 0, additionalDataLength = alert.additional_data.length; i < additionalDataLength; i++)
                            delete alert.additional_data[i].id;

                          for(var i = 0, notificationsLength = alert.notifications.length; i < notificationsLength; i++)
                            delete alert.notifications[i].id;

                          delete alert.schedule.id;
                          delete alert.automatic_schedule.id;
                          var scheduleObject = {};
                          if (alert.schedule_type == Enums.ScheduleType.AUTOMATIC){
                            scheduleObject = alert.automatic_schedule;
                            scheduleObject.scheduleType = Enums.ScheduleType.AUTOMATIC;
                          } else if (alert.schedule_type == Enums.ScheduleType.SCHEDULE){
                            scheduleObject = alert.schedule;
                            scheduleObject.scheduleType = Enums.ScheduleType.SCHEDULE;
                          } else {
                            scheduleObject.scheduleType = Enums.ScheduleType.MANUAL;
                          }

                          promises.push(
                            DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
                              if (schedule){
                                if (alert.schedule_type == Enums.ScheduleType.AUTOMATIC)
                                  alert.automatic_schedule_id = schedule.id;
                                else 
                                  alert.schedule_id = schedule.id;
                              }

                              return DataManager.addAlert(alert, options).then(function(alertResult) {
                                if(tcpOutput.Alerts === undefined) tcpOutput.Alerts = [];
                                tcpOutput.Alerts.push(alertResult);
                              });
                            })
                          );
                        });
                      }

                      return Promise.all(promises).then(function() {
                        TcpService.send(tcpOutput);
                      });
                    });
                  });
                });
              });
            });
          });
        });
      }).then(function() {
        console.log("Commited");
        client.emit("importResponse", {
          status: 200,
          data: output
        });
      }).catch(function(err){
        // remove cached data in DataManager
        if(output.Projects && output.Projects.length > 0) {
          var projects = Utils.removeAll(DataManager.data.projects, {id: {$in: output.Projects.map(function(proj) { return proj.id; }) }});
          console.log("Removed " + projects.length + " projects");
        }

        if(output.DataSeries && output.DataSeries.length > 0) {
          var ds = Utils.removeAll(DataManager.data.dataSeries, {id: {$in: output.DataSeries.map(function(ds) { return ds.id; }) }});
          console.log("Removed " + ds.length + " data series");
        }

        if(output.DataProviders && output.DataProviders.length > 0) {
          var providers = Utils.removeAll(DataManager.data.dataProviders, {id: {$in: output.DataProviders.map(function(prov) { return prov.id; }) }});
          console.log("Removed " + providers.length + " providers");
        }

        client.emit("importResponse", {
          status: 400,
          err: err.toString()
        });
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

        promises.push(DataManager.listViews({project_id: target.id}).then(function(viewsList) {
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
