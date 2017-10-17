"use strict";

/**
 * Class responsible to import a project
 * @class ImportProject
 * 
 * 
 * @property {object} json - Object with data to import.
 */
var ImportProject = function(json){

  var Utils = require('./Utils');
  var Enums = require('./Enums');
  var DataManager = require("./DataManager");
  var Promise = require("bluebird");

  if(!json || !Utils.isObject(json)) {
    return Promise.reject({status: 400, err: "Unknown error: the parameter must be a object"});
  }

  var countObjectProperties = function(object) {
    var count = 0;

    if(object !== undefined && object !== null && typeof object === "object")
      for(var key in object) if(object.hasOwnProperty(key)) count++;

    return count;
  };
  
  /**
   * @type {Array<Promise>}
   */
  var promises = [];
  var output = {};
  var tcpOutput = {};

  var _updateID = function(old, object) {
    var o = object.rawObject();
    return Object.assign({$id: old.$id}, o);
  };

  return DataManager.orm.transaction(function(t) {
    /**
     * @type {Object} options
     */
    var options = {
      transaction: t
    };
    
    var nameAlreadyInUse = function(objectName, type){
      var restriction = { name: objectName };
      switch(type){
        case 'provider':
          var providers = DataManager.listDataProviders(restriction);
          return Promise.resolve(providers.length > 0);
        case 'dataseries':
          return DataManager.listDataSeries(restriction, options).then(function(dataSeries){
            return Promise.resolve(dataSeries.length > 0);
          });
        case 'analysis':
          return DataManager.listAnalysis(restriction, options).then(function(analysis){
            return Promise.resolve(analysis.length > 0);
          });
        case 'view':
          return DataManager.listViews(restriction, options).then(function(views){
            return Promise.resolve(views.length > 0);
        });
        case 'legend':
          return DataManager.listLegends(restriction, options).then(function(legends){
            return Promise.resolve(legends.length > 0);
        });
        case 'alert':
          return DataManager.listAlerts(restriction, options).then(function(alerts){
            return Promise.resolve(alerts.length > 0);
        });
        default:
          return Promise.resolve(false);
      }
    }

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
        if (json.userId)
          project.user_id = parseInt(json.userId);
        promises.push(DataManager.addProject(project, options).then(function(proj) {
          output.Projects.push(Object.assign({ $id: project.$id }, proj));
        }));
      });
    }

    return Promise.all(promises).then(function() {
      promises = [];

      if(json.DataProviders) {
        var dataProviders = json.DataProviders || [];
        output.DataProviders = [];
        dataProviders.forEach(function(dataProvider) {
          promises.push(nameAlreadyInUse(dataProvider.name, 'provider').then(function(nameInUse){
            if (nameInUse && !thereAreProjects)
              dataProvider.name += json.selectedProject;
            dataProvider.data_provider_type_id = dataProvider.data_provider_type.id;
            dataProvider.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: dataProvider.project_id}).id : json.selectedProject;
  
            return DataManager.addDataProvider(dataProvider, options).then(function(dProvider) {
              if(tcpOutput.DataProviders === undefined) tcpOutput.DataProviders = [];
              tcpOutput.DataProviders.push(dProvider.toObject());
  
              output.DataProviders.push(_updateID(dataProvider, dProvider));
              return Promise.resolve();
            });
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
            promises.push(nameAlreadyInUse(dSeries.name, 'dataseries').then(function(nameInUse){
              if (nameInUse && !thereAreProjects)
                dSeries.name += json.selectedProject;
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
              return DataManager.addDataSeries(dSeries, null, options).then(function(dSeriesResult) {
                if(tcpOutput.DataSeries === undefined) tcpOutput.DataSeries = [];
                tcpOutput.DataSeries.push(dSeriesResult.toObject());
  
                // call helper to add IDs in output.DataSeries
                _processDataSeriesAndDataSets(dSeries, dSeriesResult);
              });
            }));
          });
        }
        return Promise.all(promises).then(function() {
          promises = [];
          // updating data sets
          output.DataSeries.forEach(function(dSeries){
            if (dSeries.dataSets){
              dSeries.dataSets.forEach(function(dataSet){
                if (dataSet.format){
                  var format = dataSet.format;
                  for (var key in format){
                    if (format.hasOwnProperty(key)){
                      if (key == "monitored_object_id"){
                        var monitoredObjectId = Utils.find(output.DataSeries, {$id: format[key]}).id;
                        format[key] = monitoredObjectId.toString();
                        var restriction = {
                          data_set_id: dataSet.id,
                          key: "monitored_object_id"
                        }
                        var dataSetFormatObject = {
                          value: monitoredObjectId
                        }
                        promises.push(DataManager.upsertDataSetFormats(restriction, dataSetFormatObject, options));
                      }
                    }
                  }
                }
              });
            }
          });
          return Promise.all(promises).then(function(){
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
                  promises.push(nameAlreadyInUse(analysis.name, 'analysis').then(function(nameInUse){
                    if (nameInUse && !thereAreProjects)
                      analysis.name += json.selectedProject;
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
  
                      return DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
                        if (analysis.schedule_type == Enums.ScheduleType.AUTOMATIC)
                          analysis.automatic_schedule_id = schedule.id;
                        else 
                          analysis.schedule_id = schedule.id;
  
                        return DataManager.addAnalysis(analysis, options).then(function(analysisResult) {
                          if(tcpOutput.Analysis === undefined) tcpOutput.Analysis = [];
                          tcpOutput.Analysis.push(analysisResult.toObject());
                        });
                      });
                    } else {
                      return DataManager.addAnalysis(analysis, options).then(function(analysisResult) {
                        if(tcpOutput.Analysis === undefined) tcpOutput.Analysis = [];
                        tcpOutput.Analysis.push(analysisResult.toObject());
                      });
                    }
                  }));
                });
              }

              return Promise.all(promises).then(function() {
                promises = [];

                if(json.Views) {
                  //check if have alert view
                  if (json.Alerts){
                    var alertsList = json.Alerts || [];
                    alertsList.forEach(function(alert){
                      var viewAlert;
                      if (alert.view && alert.view.id){
                        viewAlert = alert.view;
                        viewAlert.$id = viewAlert.id;
                        viewAlert.data_series_id = viewAlert.dataseries_id;
                        delete viewAlert.id;
                        json.Views.push(viewAlert);
                      }
                    });
                  }
                  var viewsList = json.Views || [];
                  viewsList.forEach(function(view) {
                    promises.push(nameAlreadyInUse(view.name, 'view').then(function(nameInUse){
                      if (nameInUse && !thereAreProjects)
                        view.name += json.selectedProject;

                      view.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: view.project_id}).id : json.selectedProject;
                      view.data_series_id = Utils.find(output.DataSeries, {$id: view.data_series_id}).id;
                      if(view.service_instance_id === null) view.service_instance_id = json.servicesView;
                      if (view.legend){
                        delete view.legend.id;
                        if (view.legend.colors){
                          view.legend.colors.forEach(function(color){
                            delete color.id;
                          });
                        }
                      }
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
  
                        return DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
                          if (schedule){
                            if (view.schedule_type == Enums.ScheduleType.AUTOMATIC)
                              view.automatic_schedule_id = schedule.id;
                            else 
                              view.schedule_id = schedule.id;
                          }
  
                          return DataManager.addView(view, options).then(function(viewResult) {
                            if(tcpOutput.Views === undefined) tcpOutput.Views = [];
                            var viewObject = Object.assign({$id: view.$id}, viewResult.toObject());
                            tcpOutput.Views.push(viewObject);
                          });
                        });
                      } else {
                        return DataManager.addView(view, options).then(function(viewResult) {
                          if(tcpOutput.Views === undefined) tcpOutput.Views = [];
                          var viewObject = Object.assign({$id: view.$id}, viewResult.toObject());
                          tcpOutput.Views.push(viewObject);
                        });
                      }
                    }));
                  });
                }

                return Promise.all(promises).then(function() {
                  promises = [];

                  if(json.Legends) {
                    var legendsList = json.Legends || [];
                    output.Legends = [];

                    legendsList.forEach(function(legend) {
                      promises.push(nameAlreadyInUse(legend.name, 'legend').then(function(nameInUse){
                        if (nameInUse && !thereAreProjects)
                          legend.name += json.selectedProject;

                        legend.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: legend.project_id}).id : json.selectedProject;
  
                        for(var i = 0, levelsLength = legend.levels.length; i < levelsLength; i++)
                          delete legend.levels[i].id;
  
                        return DataManager.addLegend(legend, options).then(function(legendResult) {
                          if(tcpOutput.Legends === undefined) tcpOutput.Legends = [];
                            tcpOutput.Legends.push(legendResult);

                          output.Legends.push(_updateID(legend, legendResult));
                        });
                      }));
                    });
                  }

                  return Promise.all(promises).then(function() {
                    promises = [];

                    if(json.Alerts) {

                      var alertsList = json.Alerts || [];

                      alertsList.forEach(function(alert) {
                        promises.push(nameAlreadyInUse(alert.name, 'alert').then(function(nameInUse){
                          if (nameInUse && !thereAreProjects)
                            alert.name += json.selectedProject;

                          alert.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: alert.project_id}).id : json.selectedProject;
                          alert.data_series_id = Utils.find(output.DataSeries, {$id: alert.data_series_id}).id;
                          alert.legend_id = Utils.find(output.Legends, {$id: alert.legend_id}).id;
  
                          if(alert.service_instance_id === null) alert.service_instance_id = json.servicesAlert;
                          if (alert.view && alert.view.$id){
                            var viewId = Utils.find(tcpOutput.Views, {$id: alert.view.$id}).id;
                            alert.view_id = viewId;
                          }
  
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
  
                          return DataManager.addSchedule(scheduleObject, options).then(function(schedule) {
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
                          });
                        }));
                      });
                    }
                    return Promise.all(promises).then(function(){
                      promises = [];

                      if (json.Interpolators) {
                        var interpolatorsList = json.Interpolators || [];
                        interpolatorsList.forEach(function(interpolator) {
                          interpolator.data_series_input = Utils.find(output.DataSeries, {$id: interpolator.data_series_input}).id;
                          interpolator.data_series_output = Utils.find(output.DataSeries, {$id: interpolator.data_series_output}).id;
                          interpolator.project_id = thereAreProjects ? Utils.find(output.Projects, {$id: interpolator.project_id}).id : json.selectedProject;
                          if(interpolator.service_instance_id === null) interpolator.service_instance_id = json.servicesInterpolator;
                          
                          delete interpolator.schedule.id;
                          delete interpolator.automatic_schedule.id;
                          var scheduleObject = {};
                          if (interpolator.schedule_type == Enums.ScheduleType.AUTOMATIC){
                            scheduleObject = interpolator.automatic_schedule;
                            scheduleObject.scheduleType = Enums.ScheduleType.AUTOMATIC;
                          } else if (interpolator.schedule_type == Enums.ScheduleType.SCHEDULE){
                            scheduleObject = interpolator.schedule;
                            scheduleObject.scheduleType = Enums.ScheduleType.SCHEDULE;
                          } else {
                            scheduleObject.scheduleType = Enums.ScheduleType.MANUAL;
                          }

                          promises.push(DataManager.addSchedule(scheduleObject, options).then(function(scheduleResult){
                            if (scheduleResult){
                              if (interpolator.schedule_type == Enums.ScheduleType.AUTOMATIC)
                                interpolator.automatic_schedule_id = scheduleResult.id;
                              else 
                                interpolator.schedule_id = scheduleResult.id;
                            }

                            return DataManager.addInterpolator(interpolator, options).then(function(interpolatorResult){
                              if(tcpOutput.Interpolators === undefined) tcpOutput.Interpolators = [];
                                tcpOutput.Interpolators.push(interpolatorResult.toObject());
                            });
                          }))
                        });
                      }
                      return Promise.all(promises);
                    });
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
    return Promise.resolve({status: 200, data: output, tcpOutput: tcpOutput});
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
    return Promise.reject({status: 400, err: err.toString()});
  });
}

module.exports = ImportProject;