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

  // bluebird promise
  var Promise = require("bluebird");

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
     * })
     */
    client.on("import", function(json) {
      /**
       * @type {Array<Promise>}
       */
      var promises = [];
      var output = {};
      if (!json || !Utils.isObject(json)) {
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

      // if there any project to import
      if (json.Projects) {
        var projects = json.Projects || [];
        output.Projects = [];
        projects.forEach(function(project) {
          // Try to get project by unique name
          promises.push(DataManager.getProject({name: project.name}).then(function(proj) {
            output.Projects.push(Object.assign({ $id: project.$id }, proj));
          }).catch(function(err) { // if there is not, just add 
            return DataManager.addProject(project).then(function(proj) {
              output.Projects.push(Object.assign({ $id: project.$id }, proj));
            });
          }));
        });
      }

      /**
       * Error helper for promises exception handler. It emits a client response with error
       * @param {Error} err - An exception occurred
       */
      var _emitError = function(err) {
        // TODO: rollback
        client.emit("importResponse", {
          status: 400,
          err: err.toString()
        });
      };

      Promise.all(promises).then(function() {
        promises = [];

        if (json.DataProviders) {
          var dataProviders = json.DataProviders || [];
          output.DataProviders = [];
          dataProviders.forEach(function(dataProvider) {
            // check if there is a data provider with same name
            promises.push(DataManager.getDataProvider({name: dataProvider.name}).then(function(dProvider) {
              // found. // set ID
              output.DataProviders.push(_updateID(dataProvider, dProvider));
            }).catch(function(err) {
              // not found. #insert
              console.log("DOES NOT EXISTS");
              dataProvider.data_provider_type_id = dataProvider.data_provider_type.id;
              dataProvider.project_id = Utils.find(output.Projects, {$id: dataProvider.project_id}).id;
              return DataManager.addDataProvider(dataProvider).then(function(dProvider) {
                output.DataProviders.push(_updateID(dataProvider, dProvider));
              });
            }));
          });
        }

        Promise.all(promises).then(function() {
          promises = [];
          if (json.DataSeries) {
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
              // find or create DataSeries
              promises.push(DataManager.getDataSeries({name: dSeries.name}).then(function(dSeriesResult) {
                // call helper to add IDs in output.DataSeries
                _processDataSeriesAndDataSets(dSeries, dSeriesResult);
              }).catch(function(err) {
                // preparing to insert in DataBase
                dSeries.data_series_semantic_id = dSeries.data_series_semantics.id;
                dSeries.data_provider_id = Utils.find(output.DataProviders, {$id: dSeries.data_provider_id}).id;
                dSeries.dataSets.forEach(function(dSet) {
                  dSet.$id = dSet.id;
                  delete dSet.id;
                });

                // return Promise
                return DataManager.addDataSeries(dSeries).then(function(dSeriesResult) {
                  // call helper to add IDs in output.DataSeries
                  _processDataSeriesAndDataSets(dSeries, dSeriesResult);
                });
              }));
            });
          }
          Promise.all(promises).then(function() {
            promises = [];
            if (json.Collectors) {
              var collectors = json.Collectors || [];

              collectors.forEach(function(collector) {
                collector.data_series_input = Utils.find(output.DataSeries, {$id: collector.input_data_series}).id;

                var dsOutput = Utils.find(output.DataSeries, {$id: collector.output_data_series});
                collector.data_series_output = dsOutput.id;
                promises.push(
                  DataManager.getCollector({data_series_output: dsOutput.id}).catch(function(err) {
                    // on error, try add
                    return DataManager.addSchedule(collector.schedule).then(function(scheduleResult) {
                      collector.schedule_id = scheduleResult.id;
                      return DataManager.addCollector(collector, collector.filter);
                    });
                  })
                );
              });
            }

            Promise.all(promises).then(function() {
              promises = [];

              if (json.Analysis) {
                var analysisList = json.Analysis || [];
                analysisList.forEach(function(analysis) {
                  DataManager.getAnalysis({name: analysis.name}).then(function() {
                    output.Analysis.push(analysis);
                  }).catch(function(err) {
                    return DataManager.addSchedule(analysis.schedule).then(function(schedule) {
                      analysis.schedule_id = schedule.id;

                      analysis.analysisDataSeries = analysis.analysis_dataseries_list;
                      for(var i = 0; i < analysis.analysisDataSeries.length; ++i) {
                        var ds = analysis.analysisDataSeries[i];
                        for(var k = 0; k < output.DataSeries.length; ++k) {
                          var anDs = output.DataSeries[k];
                          if (ds.data_series_id === anDs.$id) {
                            ds.type_id = ds.type;
                            ds.data_series_id = anDs.id;
                            break;
                          }
                        }
                      }

                      analysis.type_id = analysis.type.id;

                      analysis.instance_id = analysis.service_instance_id;
                      analysis.project_id = Utils.find(output.Projects, {$id: analysis.project_id}).id;
                      analysis.script_language_id = analysis.script_language;
                      analysis.grid = analysis.output_grid;
                      var dataSeriesOutput = Utils.find(output.DataSeries, {
                        dataSets: {
                          $id: analysis.output_dataseries_id
                        }
                      });

                      if (dataSeriesOutput.data_series_semantics.data_series_type_name === Enums.DataSeriesType.DCP) {
                        // TODO:
                        console.log("TODO: Analysis DCP export");
                      } else {
                        analysis.dataset_output = dataSeriesOutput.dataSets[0].id;
                      }
                      return DataManager.addAnalysis(analysis);
                    });
                  });
                });
              }

              Promise.all(promises).then(function() {
                client.emit("importResponse", {
                  status: 200,
                  data: output
                });
              }).catch(_emitError);
            }).catch(_emitError);
          }).catch(_emitError);
        }).catch(_emitError);
      }).catch(_emitError);
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
      var output = {};

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

      var promises = [];
      if (json.Projects) {
        target = json.Projects[0] || {};

        promises.push(DataManager.getProject({id: target.id}).then(function(project) {
          var projectId = project.id;
          project.$id = project.id;
          delete project.id;

          output.Projects = [project];
          output.DataProviders = [];
          output.DataSeries = [];
          output.Collectors = [];
          output.Analysis = [];
          var providers = DataManager.listDataProviders({project_id: projectId});
          providers.forEach(function(provider) {
            output.DataProviders.push(addID(provider));
          });

          DataManager.listDataSeries({dataProvider: { project_id: projectId }}).then(function(dataSeriesList) {
            dataSeriesList.forEach(function(dataSeries) {
              output.DataSeries.push(addID(dataSeries));
            });
          });
        }).catch(_emitError));

        promises.push(DataManager.listCollectors({DataProvider: {project_id: target.id}}).then(function(collectors) {
          collectors.forEach(function(collector) {
            output.Collectors.push(addID(collector));
          });
        }));

        promises.push(DataManager.listAnalyses({project_id: target.id}).then(function(analysisList) {
          analysisList.forEach(function(analysis) {
            var rawAnalysis = analysis.rawObject();
            rawAnalysis.$id = rawAnalysis.id;
            delete rawAnalysis.id;

            rawAnalysis.analysis_dataseries_list.forEach(function(analysisDS) {
              analysisDS.$id = analysisDS.id;
              delete analysisDS.id;
            });
            output.Analysis.push(rawAnalysis);
          });
        }));

      } // end if projects

      if (json.DataProviders) {
        target = json.DataProviders[0] || {};
        DataManager.getDataProvider({id: target.id}).then(function(dataProvider) {
          output.DataProviders.push(addID(dataProvider));
          DataManager.listDataSeries({dataProvider: {id: dataProvider.id}}).then(function(dataSeriesList) {
            dataSeriesList.forEach(function(dataSeries) {
              output.DataSeries.push(addID(dataSeries));
            });
          });
        });
      }

      if (json.DataSeries) {
        target = json.DataSeries[0] || {};
        DataManager.getDataSeries({id: target.id}).then(function(dataSeries) {
          output.DataSeries.push(addID(dataSeries));
          // get dependencies
          DataManager.getCollector({
            $or: {
              data_series_input: dataSeries.id,
              data_series_output: dataSeries.id
            }
          }).then(function(collector) {
            output.Collectors.push(addID(collector));

            // TODO: check if is data series generated by analysis (export dependencies)
          });
        });
      }

      Promise.all(promises).then(function() {
        client.emit("exportResponse", {status: 200, data: output});
      }).catch(_emitError);
    });
  });
};

module.exports = ImportExport;
