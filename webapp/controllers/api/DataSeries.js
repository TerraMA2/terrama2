module.exports = function(app) {
  "use strict";

  var logger = require("./../../core/Logger");
  var DataManager = require("../../core/DataManager");
  var PromiseClass = require("./../../core/Promise");
  var TcpService = require('./../../core/facade/tcp-manager/TcpService');
  var Utils = require("../../core/Utils");
  var DataSeriesError = require('../../core/Exceptions').DataSeriesError;
  var CollectorErrorNotFound = require('../../core/Exceptions').CollectorErrorNotFound;
  var DataSeriesTemporality = require('./../../core/Enums').TemporalityType;
  var TokenCode = require('./../../core/Enums').TokenCode;
  var ScheduleType = require('./../../core/Enums').ScheduleType;
  // data model
  var DataModel = require('./../../core/data-model');

  return {
    post: function(request, response) {
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceId = request.body.service;
      var intersection = request.body.intersection;
      var active = request.body.active;
      var shouldRun = request.body.run;

      DataManager.orm.transaction(function(t) {
        var options = {
          transaction: t
        };

        // check project
        return DataManager.getProject({name: (dataSeriesObject.project ? dataSeriesObject.project : dataSeriesObject.input.project)}).then(function(project) {
          if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
            dataSeriesObject.input.project_id = project.id;
            dataSeriesObject.output.project_id = project.id;
            delete dataSeriesObject.input.project;

            return DataManager.getServiceInstance({id: serviceId}, options).then(function(serviceResult) {
              return DataManager.addDataSeriesAndCollector(
                  dataSeriesObject,
                  scheduleObject,
                  filterObject,
                  serviceResult,
                  intersection,
                  active,
                  options
              ).then(function(collectorResult) {
                var collector = collectorResult.collector;
                collector.project_id = request.session.activeProject.id;

                var output = {
                  "DataSeries": [collectorResult.input.toObject(), collectorResult.output.toObject()],
                  "Collectors": [collector.toObject()]
                };

                logger.debug("OUTPUT: ", JSON.stringify(output));
                TcpService.send(output);

                return collectorResult.output;
              });
            });
          } else {
            dataSeriesObject.project_id = project.id;
            delete dataSeriesObject.project;

            return DataManager.addDataSeries(dataSeriesObject, options).then(function(dataSeriesResult) {
              var output = {
                "DataSeries": [dataSeriesResult.toObject()]
              };

              logger.debug("OUTPUT: ", JSON.stringify(output));

              TcpService.send(output);

              return dataSeriesResult;
            });
          }
        });
      }).then(function(dataSeriesResult) {
        var extra = {};
        if (shouldRun && dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')){
          extra = {
            id: dataSeriesResult.id
          }
        }
        var token = Utils.generateToken(app, TokenCode.SAVE, dataSeriesResult.name, extra);
        return response.json({status: 200, token: token});
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    get: function(request, response) {
      var dataProvider = request.params.dataProvider;
      var project = request.params.project;
      var ignoreAnalysisOutputDataSeries = request.query.ignoreAnalysisOutputDataSeries;
      var ignoreInterpolatorOutputDataSeries = request.query.ignoreInterpolatorOutputDataSeries;

      var dataSeriesId = request.params.id;
      var dataSeriesTemporality = request.query.type;
      var schema = request.query.schema;

      // collector scope
      var collector = request.query.collector;

      var dataSeriesTemporalityName;

      // list data series restriction
      if(dataProvider) {
        var restriction = {
          dataProvider: {
            id: dataProvider
          }
        };
      } else if(project) {
        var restriction = {
          dataProvider: {
            project_id: project
          }
        };
      } else {
        var restriction = {
          dataProvider: {
            project_id: request.session.activeProject.id
          }
        };
      }

      if (dataSeriesTemporality) {
        // checking data series: static or dynamic to filter data series output
        switch(dataSeriesTemporality) {
          case "static":
            dataSeriesTemporalityName = DataSeriesTemporality.STATIC;
            break;
          case "dynamic":
            dataSeriesTemporalityName = {
              $ne: DataSeriesTemporality.STATIC
            };
            break;
          default:
            return Utils.handleRequestError(response, new DataSeriesError("Invalid data series type. Available: \'static\' and \'dynamic\'"), 400);
        }

        restriction.data_series_semantics = {
          temporality: dataSeriesTemporalityName
        };
      }

      if (!schema) {
        if (collector) {
          restriction.Collector = {};
        }
      } else {
        restriction.schema = schema;
      }

      if (dataSeriesId) {
        DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeries) {
          return response.json(dataSeries.toObject());
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        DataManager.listDataSeries(restriction).then(function(dataSeriesList) {
          var output = [];

          if(ignoreAnalysisOutputDataSeries == true || ignoreAnalysisOutputDataSeries == 'true' || ignoreInterpolatorOutputDataSeries == true || ignoreInterpolatorOutputDataSeries == "true") {
            DataManager.listAnalysis({}).then(function(analysisList) {
              DataManager.listInterpolators({}).then(function(interpolatorsList){
                dataSeriesList.forEach(function(dataSeries) {
                  var addDataSeries = true;
                  if(ignoreAnalysisOutputDataSeries == true || ignoreAnalysisOutputDataSeries == 'true'){
                    analysisList.map(function(analysis) {
                      dataSeries.dataSets.map(function(dataSet) {
                        if(analysis.dataset_output == dataSet.id) {
                          addDataSeries = false;
                          return;
                        }
                      });
    
                      if(!addDataSeries) return;
                    });
                  }

                  if ((ignoreInterpolatorOutputDataSeries == true || ignoreInterpolatorOutputDataSeries == "true") && addDataSeries){
                    interpolatorsList.map(function(interpolator){
                      if (dataSeries.id == interpolator.data_series_output){
                        addDataSeries = false;
                        return;
                      }
                    })
                  }
  
                  if(addDataSeries) output.push(dataSeries.rawObject());
                });
  
                response.json(output);
              });
            });
          } else {
            DataManager.listAnalysis({}).then(function(analysisList){
              dataSeriesList.forEach(function(dataSeries) {
                var isAnalysis = false;
                analysisList.map(function(analysis){
                  dataSeries.dataSets.map(function(dataSet){
                    if(analysis.dataset_output == dataSet.id) {
                      isAnalysis = true;
                      return;
                    }
                  });
                });
                var dataSeriesRaw = dataSeries.rawObject();
                dataSeriesRaw.isAnalysis = isAnalysis;
                output.push(dataSeriesRaw);
              });
              response.json(output);
            });
          }
        }).catch(function(err) {
          logger.error(err);
          return Utils.handleRequestError(response, err, 400);
        });
      }
    },

    put: function(request, response) {
      var dataSeriesId = request.params.id;
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceId = request.body.service;
      var intersection = request.body.intersection;
      var shouldRun = request.body.run;
      var active = request.body.active;

      DataManager.orm.transaction(function(t) {
        var options = {
          transaction: t
        };

        if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
          dataSeriesObject.input.project_id = request.session.activeProject.id;
          return DataManager.getCollector({data_series_input: dataSeriesId}, options)
            .then(function(collector) {
              collector.service_instance_id = serviceId;
              collector.active = dataSeriesObject.input.active;
              collector.schedule_type = scheduleObject.scheduleType;

              var updateSchedulePromise

              var oldScheduleType = collector.scheduleType;
              var newScheduleType = scheduleObject.scheduleType;
              var removeSchedule = false;
              var scheduleIdToRemove;

              if (oldScheduleType == newScheduleType){
                if (newScheduleType == ScheduleType.MANUAL){
                  updateSchedulePromise = Promise.resolve();
                } else {
                  updateSchedulePromise = DataManager.updateSchedule(collector.schedule.id, scheduleObject, options);
                }
              } else {
                if (newScheduleType == ScheduleType.MANUAL) {
                  scheduleIdToRemove = collector.schedule.id;
                  collector.schedule_id = null;
                  removeSchedule = true;
                  updateSchedulePromise = Promise.resolve();
                } else {
                  updateSchedulePromise = DataManager.addSchedule(scheduleObject, options).then(function(newSchedule){
                    collector.schedule_id = newSchedule.id;
                    collector.schedule = newSchedule;
                  });
                }
              }
              return updateSchedulePromise.then(function(){

                return DataManager.updateCollector(collector.id, collector, options)
                  .then(function() {
                    // input
                    return DataManager.updateDataSeries(parseInt(dataSeriesId), dataSeriesObject.input, options);
                  })
                  // try update data series output
                  .then(function() {
                    return DataManager.updateDataSeries(parseInt(collector.data_series_output), dataSeriesObject.output, options);
                  })
                  // verify if must remove a schedule
                  .then(function() {
                    if (removeSchedule){
                      return DataManager.removeSchedule({id: scheduleIdToRemove}, options);
                    } else {
                      return Promise.resolve();
                    }
                  })
                  // try update filter
                  .then(function() {
                    // if there is a filter registered, tries to update/delete
                    if (collector.filter.id) {
                      var filterUpdate = Object.assign(collector.filter.rawObject(), filterObject);
                      if (!filterObject.region) {
                        filterUpdate.region = null;
                      }

                      if (!filterObject.data_series_id) {
                        filterUpdate.data_series_id = null;
                      }

                      if (!Utils.isEmpty(filterObject.date)) {
                        if (!filterObject.date.beforeDate) {
                          filterUpdate.discard_before = null;
                          delete filterUpdate.date.beforeDate;
                        }
                        if (!filterObject.date.afterDate) {
                          filterUpdate.discard_after = null;
                          delete filterUpdate.date.afterDate;
                        }
                      }

                      return DataManager.updateFilter(collector.filter.id, filterUpdate, options)
                        .then(function() {
                          return DataManager.getFilter({id: collector.filter.id}, options)
                            .then(function(filter) {
                              collector.filter = filter;
                            });
                        });
                    } else {
                      if (Utils.isEmpty(filterObject.date) && filterObject.filterArea == "1") {
                        return null;
                      } else {
                        filterObject.collector_id = collector.id;

                        return DataManager.addFilter(filterObject, options)
                          .then(function(filter) {
                            collector.filter = filter;
                          });
                      }
                    }
                  })
                  // try to update intersection
                  .then(function() {
                    // temp: remove all and insert. TODO: sequelize upsert / delete
                    if (Utils.isEmpty(intersection)) {
                      return DataManager.removeIntersection({collector_id: collector.id}, options)
                        .then(function() {
                          collector.setIntersection([]);
                          return collector;
                        });
                    } else {
                      return DataManager.removeIntersection({collector_id: collector.id}, options)
                        .finally(function() {
                          intersection.forEach(function(intersect) {
                            intersect.collector_id = collector.id;
                          });

                          return DataManager.addIntersection(intersection, options)
                            .then(function(intersectionResult) {
                              collector.setIntersection(intersectionResult);
                              return collector;
                            });
                        });
                    }
                  })
                  // retrieve updated data series input and output
                  .then(function() {
                    return PromiseClass.all([
                        DataManager.getDataSeries({id: collector.data_series_output}),
                        DataManager.getDataSeries({id: collector.data_series_input})
                      ]);
                  })
                  // send via TCP
                  .then(function(dSeries) {
                    var dataSeriesOutput = dSeries[0];
                    var dataSeriesInput = dSeries[1];

                    collector.project_id = request.session.activeProject.id;
                    var output = {
                      "DataSeries": [dataSeriesInput.toObject(), dataSeriesOutput.toObject()],
                      "Collectors": [collector.toObject()]
                    };

                    // tcp sending
                    TcpService.send(output);

                    return dataSeriesOutput;
                  });

              });
          })
          .catch(function(err){
            if (err instanceof CollectorErrorNotFound){
              return DataManager.getServiceInstance({id: serviceId}, options).then(function(serviceResult){
                return DataManager.updateDataSeries(dataSeriesId, dataSeriesObject.output, options).then(function(){
                  return DataManager.getDataSeries({id: dataSeriesId}).then(function(outputDataSeries){
                    return DataManager.addDataSeries(dataSeriesObject.input, null, options).then(function(inputDataSeries){
                      var addSchedulePromise;
                      if (scheduleObject.scheduleType == ScheduleType.MANUAL){
                        addSchedulePromise = Promise.resolve();
                      } else {
                        addSchedulePromise = DataManager.addSchedule(scheduleObject, options);
                      }
                      return addSchedulePromise.then(function(scheduleResult){
                        var collectorObject = {};

                        collectorObject.data_series_input = inputDataSeries.id;
                        collectorObject.data_series_output = dataSeriesId;
                        collectorObject.service_instance_id = serviceResult.id;
                        collectorObject.active = active;
                        collectorObject.collector_type = 1;
                        collectorObject.schedule_type = scheduleObject.scheduleType;
                        if (scheduleObject.scheduleType == ScheduleType.SCHEDULE){
                          collectorObject.schedule_id = scheduleResult.id;
                        }

                        return DataManager.addCollector(collectorObject, filterObject, options).then(function(collectorResult){
                          if (!intersection){
                            var collector = {
                              collector: collectorResult,
                              input: inputDataSeries,
                              output: outputDataSeries,
                              schedule: scheduleResult
                            };

                            var output = {
                              "DataSeries": [collector.input.toObject(), collector.output.toObject()],
                              "Collectors": [collectorResult.toObject()]
                            };
                            TcpService.send(output);
                            
                            return collector.output;
                          } else {
                            intersection.forEach(function(intersect) {
                              intersect.collector_id = collectorResult.id;
                            });
                            return DataManager.addIntersection(intersection, options).then(function(bulkIntersectionResult){
                              collectorResult.setIntersection(bulkIntersectionResult);
                              var collector = {
                                collector: collectorResult,
                                input: inputDataSeries,
                                output: outputDataSeries,
                                schedule: scheduleResult,
                                intersection: bulkIntersectionResult
                              };

                              var output = {
                                "DataSeries": [collector.input.toObject(), collector.output.toObject()],
                                "Collectors": [collectorResult.toObject()]
                              };
                              TcpService.send(output);

                              return collector.output;
                            });
                          }
                        });
                      });
                    });
                  });
                });
              });
            }
          });
        } else {
          return DataManager.getCollector({data_series_input: dataSeriesId}, options)
            .then(function(collector){
              var inputDataSeriesId = collector.data_series_input;
              var outputDataSeriesId = collector.data_series_output;
              return DataManager.removeDataSerie({id: inputDataSeriesId})
                .then(function(){
                  return DataManager.removeSchedule({id: collector.schedule.id})
                    .then(function(){
                      return DataManager.updateDataSeries(outputDataSeriesId, dataSeriesObject, options)
                        .then(function() {
                          return DataManager.getDataSeries({id: outputDataSeriesId})
                            .then(function(dataSeries) {
                              // tcp sending
                              TcpService.send({
                                "DataSeries": [dataSeries.toObject()]
                              });
                              return dataSeries;
                            });
                        });
                    });
                });
            }).catch(function(err){
              if (err instanceof CollectorErrorNotFound){
                return DataManager.updateDataSeries(dataSeriesId, dataSeriesObject, options)
                  .then(function() {
                    return DataManager.getDataSeries({id: dataSeriesId})
                      .then(function(dataSeries) {
                        // tcp sending
                        TcpService.send({
                          "DataSeries": [dataSeries.toObject()]
                        });
                        return dataSeries;
                      });
                  });
              } else {
                return Utils.handleRequestError(response, err, 400);
              }
            });
        }
      })
      // on success (transaction commit)
      .then(function(dataSeries) {
        var extra = {};
        if (shouldRun && dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')){
          extra = {
            id: dataSeries.id
          }
        }
        var token = Utils.generateToken(app, TokenCode.UPDATE, dataSeries.name, extra);
        return response.json({status: 200, result: dataSeries.toObject(), token: token});
      })

      .catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    delete: function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.listAnalysisDataSeries({data_series_id: id})
          .then(function(analysisDataSeriesList){
            if (analysisDataSeriesList.length > 0) {
              var analysisIds = [];
              analysisDataSeriesList.forEach(function(analysisDataSeries){
                analysisIds.push(analysisDataSeries.analysis_id);
              });
              DataManager.listAnalysis({id: {$in: analysisIds}}).then(function(analysisList){
                var analysisName = [];
                analysisList.forEach(function(analysis){
                  analysisName.push(analysis.name);
                });
                Utils.handleRequestError(response, { message: "Could not remove data series, using in " + analysisName.join(", ")}, 400);
                
              }).catch(function(error){
                Utils.handleRequestError(response, error, 400);
              });
            }
            else {
              return DataManager.getDataSeries({id: id})
                .then(function(dataSeriesResult) {
                  return DataManager.getCollector({data_series_output: id})
                    .then(function(collectorResult) {
                      return PromiseClass.all([
                          DataManager.removeDataSerie({id: id}),
                          DataManager.removeDataSerie({id: collectorResult.data_series_input}),
                          DataManager.removeSchedule({id: collectorResult.schedule.id})
                        ])
                        .then(function() {
                          var objectToSend = {
                            "Collectors": [collectorResult.id],
                            "DataSeries": [collectorResult.data_series_input, collectorResult.data_series_output],
                            "Schedule": [collectorResult.schedule.id]
                          };

                          TcpService.remove(objectToSend);

                          return response.json({status: 200, name: dataSeriesResult.name});
                        });
                    })

                    .catch(function(err) {
                      // if not find collector, check if is from interpolator
                      return DataManager.getInterpolator({data_series_output: id})
                        .then(function(interpolatorResult){
                          return DataManager.removeInterpolator({id: interpolatorResult.id})
                            .then(function(){
                              var objectToSend = {
                                "Interpolators": [interpolatorResult.id],
                                "DataSeries": [id]
                              };
                              TcpService.remove(objectToSend);
                              return response.json({status: 200, name: dataSeriesResult.name});
                            });
                        }).catch(function(err){
                          // if not find collector, it is processing data series or analysis data series
                          return DataManager.removeDataSerie({id: id})
                            .then(function() {
                              var objectToSend = {
                                "DataSeries": [id]
                              };
    
                              TcpService.remove(objectToSend);
    
                              return response.json({status: 200, name: dataSeriesResult.name});
                            });
                        })
                    })
                })
                .catch(function(error) {
                  Utils.handleRequestError(response, error, 400);
                });
            }
          })
          .catch(function(error){
            Utils.handleRequestError(response, error, 400);
          });
      } else {
        Utils.handleRequestError(response, new DataSeriesError("Missing dataseries id"), 400);
      }
    }
  };
};
