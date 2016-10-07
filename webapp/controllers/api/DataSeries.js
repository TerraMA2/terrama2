"use strict";

var DataManager = require("../../core/DataManager");
var TcpManager = require('../../core/TcpManager');
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;
var DataSeriesType = require('./../../core/Enums').DataSeriesType;
var TokenCode = require('./../../core/Enums').TokenCode;
var _ = require('lodash');

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceId = request.body.service;
      var intersection = request.body.intersection;
      var active = request.body.active;

      DataManager.orm.transaction(function(t) {
        var options = {
          transaction: t
        };
        if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
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
              collector.project_id = app.locals.activeProject.id;

              var output = {
                "DataSeries": [collectorResult.input.toObject(), collectorResult.output.toObject()],
                "Collectors": [collector.toObject()]
              };

              console.log("OUTPUT: ", JSON.stringify(output));

              Utils.sendDataToServices(DataManager, TcpManager, output);

              return collectorResult.output;
            });
          });
        } else {
          return DataManager.addDataSeries(dataSeriesObject, options).then(function(dataSeriesResult) {
            var output = {
              "DataSeries": [dataSeriesResult.toObject()]
            };

            console.log("OUTPUT: ", JSON.stringify(output));

            Utils.sendDataToServices(DataManager, TcpManager, output);

            return dataSeriesResult;
          });
        }
      }).then(function(dataSeriesResult) {
        var token = Utils.generateToken(app, TokenCode.SAVE, dataSeriesResult.name);
        return response.json({status: 200, token: token});
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    get: function(request, response) {
      var dataSeriesId = request.params.id;
      var dataSeriesType = request.query.type;
      var schema = request.query.schema;

      // collector scope
      var collector = request.query.collector;

      var dataSeriesTypeName;

      // list data series restriction
      var restriction = {
        dataProvider: {
          project_id: app.locals.activeProject.id
        }
      };

      if (dataSeriesType) {
        // checking data series: static or dynamic to filter data series output
        switch(dataSeriesType) {
          case "static":
            dataSeriesTypeName = DataSeriesType.STATIC_DATA;
            break;
          case "dynamic":
            dataSeriesTypeName = {
              $ne: DataSeriesType.STATIC_DATA
            };
            break;
          default:
            return Utils.handleRequestError(response, new DataSeriesError("Invalid data series type. Available: \'static\' and \'dynamic\'"), 400);
        }

        restriction.data_series_semantics = {
          data_series_type_name: dataSeriesTypeName
        };
      }

      if (!schema) {
        if (collector) {
          console.log("has collector ", collector);
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
          dataSeriesList.forEach(function(dataSeries) {
            output.push(dataSeries.rawObject());
          });
          response.json(output);
        }).catch(function(err) {
          console.log(err);
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

      DataManager.orm.transaction(function(t) {
        var options = {
          transaction: t
        };

        if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
          return DataManager.getCollector({data_series_input: dataSeriesId}, options)
            .then(function(collector) {
              collector.service_instance_id = serviceId;

              return DataManager.updateCollector(collector.id, collector, options)
                .then(function() {
                  // input
                  return DataManager.updateDataSeries(parseInt(dataSeriesId), dataSeriesObject.input, options);
                })
                // try update data series output
                .then(function() {
                  return DataManager.updateDataSeries(parseInt(collector.data_series_output), dataSeriesObject.output, options);
                })
                // try update schedule
                .then(function() {
                  return DataManager.updateSchedule(collector.schedule.id, scheduleObject, options)
                    .then(function() {
                      collector.schedule = new DataManager.DataModel.Schedule(Utils.extend({id: collector.schedule.id}, scheduleObject));
                      return collector;
                    });
                })
                // try update filter
                .then(function() {
                  // if there is a filter registered, tries to update/delete
                  if (collector.filter.id) {
                    var filterUpdate = Object.assign(collector.filter.rawObject(), filterObject);
                    if (!filterObject.region) {
                      filterUpdate.region = null;
                    }

                    if (!_.isEmpty(filterObject.date)) {
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
                    if (_.isEmpty(filterObject.date)) {
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
                  if (_.isEmpty(intersection)) {
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
                  return Promise.all([
                      DataManager.getDataSeries({id: collector.data_series_output}),
                      DataManager.getDataSeries({id: collector.data_series_input})
                    ]);
                })
                // send via TCP
                .then(function(dSeries) {
                  var dataSeriesOutput = dSeries[0];
                  var dataSeriesInput = dSeries[1];

                  collector.project_id = app.locals.activeProject.id;
                  var output = {
                    "DataSeries": [dataSeriesInput.toObject(), dataSeriesOutput.toObject()],
                    "Collectors": [collector.toObject()]
                  };

                  // tcp sending
                  Utils.sendDataToServices(DataManager, TcpManager, output);

                  return dataSeriesOutput;
                });
          });
        } else {
          return DataManager.updateDataSeries(dataSeriesId, dataSeriesObject, options)
            .then(function() {
              return DataManager.getDataSeries({id: dataSeriesId})
                .then(function(dataSeries) {
                  // tcp sending
                  Utils.sendDataToServices(DataManager, TcpManager, {
                    "DataSeries": [dataSeries.toObject()]
                  });
                  return dataSeries;
                });
            });
        }
      })
      // on success (transaction commit)
      .then(function(dataSeries) {
        var token = Utils.generateToken(app, TokenCode.UPDATE, dataSeries.name);
        return response.json({status: 200, result: dataSeries.toObject(), token: token});
      })
      
      .catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    delete: function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getDataSeries({id: id}).then(function(dataSeriesResult) {
          DataManager.getCollector({data_series_output: id}).then(function(collectorResult) {
            DataManager.removeDataSerie({id: id}).then(function() {
              DataManager.removeDataSerie({id: collectorResult.data_series_input}).then(function() {
                DataManager.removeSchedule({id: collectorResult.schedule.id}).then(function() {
                  var objectToSend = {
                    "Collectors": [collectorResult.id],
                    "DataSeries": [collectorResult.data_series_input, collectorResult.data_series_output],
                    "Schedule": [collectorResult.schedule.id]
                  };

                  if (Object.keys(collectorResult.intersection).length > 0) {
                    // TODO: add intersection in object to send
                  }

                  DataManager.listServiceInstances().then(function(services) {
                    services.forEach(function (service) {
                      try {
                        TcpManager.emit('removeData', service, objectToSend);
                      } catch (e) {
                        console.log(e);
                      }
                    });

                    return response.json({status: 200, name: dataSeriesResult.name});
                  }).catch(function(err) {
                    return Utils.handleRequestError(response, err, 400);
                  });
                }).catch(function(err) {
                  Utils.handleRequestError(response, err, 400);
                });
              }).catch(function(err) {
                Utils.handleRequestError(response, err, 400);
              });
            }).catch(function(err) {
              Utils.handleRequestError(response, err, 400);
            });
          }).catch(function(err) {
            // if not find collector, it is processing data series or analysis data series
            DataManager.removeDataSerie({id: dataSeriesResult.id}).then(function() {

              var objectToSend = {
                "DataSeries": [dataSeriesResult.id]
              };

              DataManager.listServiceInstances().then(function(services) {
                services.forEach(function (service) {
                  try {
                    TcpManager.emit('removeData', service, objectToSend);
                  } catch (e) {
                    console.log(e);
                  }
                });

                response.json({status: 200, name: dataSeriesResult.name});
              }).catch(function(err) {
                return Utils.handleRequestError(response, err, 400);
              });

            }).catch(function(error) {
              Utils.handleRequestError(response, error, 400);
            });
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      } else {
        Utils.handleRequestError(response, new DataSeriesError("Missing dataseries id"), 400);
      }
    }
  };
};
