"use strict";

/**
 * Class responsible to export a project
 * @class ExportProject
 * 
 * @property {object} json - Object with data to export.
 */
var ExportProject = function(json){

  var DataManager = require("./DataManager");
  var Promise = require("bluebird");

  var countObjectProperties = function(object) {
    var count = 0;
    
    if(object !== undefined && object !== null && typeof object === "object")
      for(var key in object) if(object.hasOwnProperty(key)) count++;

    return count;
  };

  var output = {
    Projects: [],
    DataProviders: [],
    DataSeries: [],
    Collectors: [],
    Analysis: [],
    Views: [],
    Alerts: [],
    Legends: [],
    Interpolators: []
  };

  var _emitError = function(err) {
    return Promise.reject({err: err.toString(), status: 400});
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
      delete project.user_id;

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

    promises.push(DataManager.listInterpolators({project_id: target.id}).then(function(interpolators){
      interpolators.forEach(function(interpolator){
        var interpolatorToAdd = addID(interpolator);

        if(countObjectProperties(interpolatorToAdd.schedule) > 0)
          interpolatorToAdd.schedule.scheduleType = interpolatorToAdd.schedule_type;

        output.Interpolators.push(interpolatorToAdd);
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

  if (json.Interpolators) {
    for (var i = 0, interpolatorsLength = json.Interpolators.length; i < interpolatorsLength; i++){
      promises.push(
        DataManager.getInterpolator({id: json.Interpolators[i].id}).then(function(interpolator){
          if (!isInArray(interpolator.id, output.Interpolators)){
            var rawInterpolator = interpolator.rawObject();
            rawInterpolator.$id = rawInterpolator.id;
            delete rawInterpolator.id;
            rawInterpolator.project_id = null;
            rawInterpolator.service_instance_id = null;

            if(countObjectProperties(rawInterpolator.schedule) > 0)
            rawInterpolator.schedule.scheduleType = rawInterpolator.schedule_type;

            output.Interpolators.push(rawInterpolator);
            
            var interpolatorsDataseriesListPromises = [getDataSeries(rawInterpolator.data_series_output)];

            return Promise.all(interpolatorsDataseriesListPromises).catch(_emitError);
          } else {
            return Promise.resolve();
          }
        })
      );
    }
  }

  return Promise.all(promises).then(function() {
    if(output.Projects.length === 0) delete output.Projects;
    if(output.DataProviders.length === 0) delete output.DataProviders;
    if(output.DataSeries.length === 0) delete output.DataSeries;
    if(output.Collectors.length === 0) delete output.Collectors;
    if(output.Analysis.length === 0) delete output.Analysis;
    if(output.Views.length === 0) delete output.Views;
    if(output.Alerts.length === 0) delete output.Alerts;
    if(output.Legends.length === 0) delete output.Legends;

    return Promise.resolve({ status: 200, data: output, projectName: json.currentProjectName, fileName: json.fileName});
  }).catch(_emitError);

}

module.exports = ExportProject;