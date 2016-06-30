var DataManager = require("../../core/DataManager");
var TcpManagerClass = require('../../core/TcpManager');
var Utils = require("../../core/Utils");
var DataSeriesError = require('../../core/Exceptions').DataSeriesError;
var DataSeriesType = require('./../../core/Enums').DataSeriesType;
var TokenCode = require('./../../core/Enums').TokenCode;
var isEmpty = require('lodash').isEmpty;
var passport = require('./../../config/Passport');

module.exports = function(app) {
  var TcpManager = new TcpManagerClass();

  return {
    post: [passport.isCommonUser, function(request, response) {
      var dataSeriesObject = request.body.dataSeries;
      var scheduleObject = request.body.schedule;
      var filterObject = request.body.filter;
      var serviceId = request.body.service;
      var intersection = request.body.intersection;
      var active = request.body.active;

      if (dataSeriesObject.hasOwnProperty('input') && dataSeriesObject.hasOwnProperty('output')) {
        DataManager.getServiceInstance({id: serviceId}).then(function(serviceResult) {
          DataManager.addDataSeriesAndCollector(
            dataSeriesObject,
            scheduleObject,
            filterObject,
            serviceResult,
            intersection,
            active
          ).then(function(collectorResult) {
            var collector = collectorResult.collector;
            collector['project_id'] = app.locals.activeProject.id;

            var output = {
              "DataSeries": [collectorResult.input.toObject(), collectorResult.output.toObject()],
              "Collectors": [collector.toObject()]
            };

            console.log("OUTPUT: ", JSON.stringify(output));

            DataManager.listServiceInstances().then(function(servicesInstance) {
              servicesInstance.forEach(function (service) {
                try {
                  TcpManager.emit('sendData', service, output);
                } catch (e) {
                  console.log("Error during send data each service: ", e);
                }
              });
              TcpManager.emit('removeListeners');

              var token = Utils.generateToken(app, TokenCode.SAVE, collectorResult.output.name);
              return response.json({status: 200, output: output, token: token});
            }).catch(function(err) {
              return Utils.handleRequestError(response, err, 400);
            })
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          });
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      } else {
        DataManager.addDataSeries(dataSeriesObject).then(function(dataSeriesResult) {
          var output = {
            "DataSeries": [dataSeriesResult.toObject()]
          };

          console.log("OUTPUT: ", JSON.stringify(output));

          DataManager.listServiceInstances().then(function(servicesInstance) {
            servicesInstance.forEach(function (service) {
              try {
                TcpManager.emit('sendData', service, output);
              } catch (e) {
                console.log("Error during send data each service: ", e);
              }
            });
            TcpManager.emit('removeListeners');

            var token = Utils.generateToken(app, TokenCode.SAVE, dataSeriesResult.name);
            return response.json({status: 200, output: output, token: token});
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          })
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }
    }],

    get: function(request, response) {
      var dataSeriesId = request.params.id;
      var dataSeriesType = request.query.type;
      var schema = request.query.schema;

      // collector scope
      var collector = request.query['collector'];

      var dataSeriesTypeName;

      // list dataseries restriction
      var restriction = {};

      if (dataSeriesType) {
        // checking data series: static or dynamic to filter data series output
        switch(dataSeriesType) {
          case "static":
            dataSeriesTypeName = DataSeriesType.STATIC_DATA;
            break;
          case "dynamic":
            break;
          default:
            return Utils.handleRequestError(response, new DataSeriesError("Invalid data series type. Available: \'static\' and \'dynamic\'"), 400);
        }

        restriction.DataSeriesSemantics = {
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

    },

    delete: [passport.isCommonUser, function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getDataSeries({id: id}).then(function(dataSeriesResult) {
          DataManager.getCollector({data_series_output: id}).then(function(collectorResult) {
            DataManager.removeDataSerie({id: id}).then(function() {
              DataManager.removeDataSerie({id: collectorResult.input_data_series}).then(function() {
                DataManager.removeSchedule({id: collectorResult.schedule.id}).then(function() {
                  var objectToSend = {
                    "Collectors": [collectorResult.id],
                    "DataSeries": [collectorResult.input_data_series, collectorResult.output_data_series],
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
                    TcpManager.emit('removeListeners');

                    return response.json({status: 200, name: dataSeriesResult.name});
                  }).catch(function(err) {
                    return Utils.handleRequestError(response, err, 400);
                  })
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
              }

              DataManager.listServiceInstances().then(function(services) {
                services.forEach(function (service) {
                  try {
                    TcpManager.emit('removeData', service, objectToSend);
                  } catch (e) {
                    console.log(e);
                  }
                });
                TcpManager.emit('removeListeners');

                response.json({status: 200, name: dataSeriesResult.name});
              }).catch(function(err) {
                return Utils.handleRequestError(response, err, 400);
              })

            }).catch(function(error) {
              Utils.handleRequestError(response, error, 400);
            })
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        })
      } else {
        Utils.handleRequestError(response, new DataSeriesError("Missing dataseries id"), 400);
      }
    }]
  };
};
