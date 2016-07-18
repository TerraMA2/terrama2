var Enums = require("./Enums");
var FormField = Enums.Form.Field;
var UriPattern = Enums.Uri;
var cloneDeep = require("lodash").cloneDeep;
var crypto = require('crypto');
var exceptions = require('./Exceptions');
var Signals = require('./Signals');
var Promise = require("bluebird");
var util = require('util');
var isEqual = require('lodash').isEqual;

// nodejs
var glob = require('glob');
var path = require('path');

function getTokenCodeMessage(code) {
  var msg;
  switch(code) {
    case Enums.TokenCode.SAVE:
      msg = "saved";
      break;
    case Enums.TokenCode.UPDATE:
      msg = "updated";
      break;
    case Enums.TokenCode.DELETE:
      msg = "deleted";
      break;
    default:
      msg = "";
      break;
  }
  return msg;
}

module.exports = {
  clone: function(object) {
    return cloneDeep(object);
  },

  handleRequestError: function(response, err, code) {
    response.status(code);
    return response.json({status: code, message: err.message});
  },

  getCommonRequestFields : function() {
    return [{
      key: UriPattern.HOST,
      type: FormField.TEXT,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.PORT,
      type: FormField.NUMBER,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.USER,
      type: FormField.TEXT,
      htmlClass: 'col-md-6 terrama2-schema-form'
    },
    {
      key: UriPattern.PASSWORD,
      type: FormField.PASSWORD,
      htmlClass: 'col-md-6 terrama2-schema-form'
    }]
  },

  makeCommonRequestFields: function(scheme, port, exceptField, required, displayOrder) {
    var properties = {};
    properties[UriPattern.HOST] = {
      title: "Address",
      type: FormField.TEXT
    };

    properties[UriPattern.PORT] = {
      title: "Port",
      type: FormField.NUMBER,
      default: port
    };

    properties[UriPattern.USER] = {
      title: "Username",
      type: FormField.TEXT
    };

    properties[UriPattern.PASSWORD] = {
      title: "Password",
      type: FormField.TEXT
    };

    properties[exceptField || UriPattern.PATHNAME] = {
      title: (exceptField ? exceptField[0].toUpperCase() + exceptField.slice(1) : exceptField) || "Path",
      type: FormField.TEXT
    };

    return {
      name: scheme,

      properties: properties,
      required: required,
      display: displayOrder
    }
  },

  rollback: function(model, instance) {
    return model.destroy({
      where: {
        id: instance.id
      }
    })
  },

  rollbackModels: function(models, instances, exception, promise) {
    var promises = [];
    for(var i = 0; i < models.length; ++i) {
      promises.push(this.rollback(models[i], instances[i]));
    }

    Promise.all(promises).then(function() {
      console.log("Rollback all");
      return promise.reject(exception);
    }).catch(function(err) {
      promise.reject(err);
    })
  },

  rollbackPromises: function(promises, exception, errorHandler) {
    Promise.all(promises).then(function() {
      errorHandler(exception);
    }).catch(function(err) {
      errorHandler(err);
    })
  },

  generateToken: function(app, code, intent) {
    var token = crypto.randomBytes(48).toString('hex');
    app.locals.tokenIntent = {
      token: token,
      code: code,
      intent: intent
    };

    return token;
  },

  getTokenCodeMessage: getTokenCodeMessage,

  makeTokenParameters: function(token, app) {
    var parameters = {};

    if (app.locals.tokenIntent && app.locals.tokenIntent.token === token) {
      var code = app.locals.tokenIntent.code;
      var intent = app.locals.tokenIntent.intent;

      parameters.message = intent + " " + getTokenCodeMessage(code);
      // resetting
      delete app.locals.tokenIntent;
    }

    return parameters;
  },

  getAnalysisType: function(analysisCode) {
    if (analysisCode) {
      switch(parseInt(analysisCode)) {
        case Enums.AnalysisType.DCP:
          return Enums.DataSeriesType.DCP;
          break;
        case Enums.AnalysisType.GRID:
          return Enums.DataSeriesType.GRID;
          break;
        case Enums.AnalysisType.MONITORED:
          return Enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT;
          break;
        default:
          throw new Error("Invalid analysis id");
          break;
      }
    }
    throw new Error("Invalid analysis id");
  },

  getTcpSignal: function(value) {
    switch(value) {
      case Signals.TERMINATE_SERVICE_SIGNAL:
        return Signals.TERMINATE_SERVICE_SIGNAL;
        break;
      case Signals.STATUS_SIGNAL:
        return Signals.STATUS_SIGNAL;
        break;
      case Signals.ADD_DATA_SIGNAL:
        return Signals.ADD_DATA_SIGNAL;
        break;
      case Signals.START_PROCESS_SIGNAL:
        return Signals.START_PROCESS_SIGNAL;
        break;
      case Signals.LOG_SIGNAL:
        return Signals.LOG_SIGNAL;
        break;
      case Signals.REMOVE_DATA_SIGNAL:
        return Signals.REMOVE_DATA_SIGNAL;
        break;
      case Signals.PROCESS_FINISHED_SIGNAL:
        return Signals.PROCESS_FINISHED_SIGNAL;
        break;
      case Signals.UPDATE_SERVICE_SIGNAL:
        return Signals.UPDATE_SERVICE_SIGNAL;
        break;
      default:
        throw new exceptions.SignalError("Invalid terrama2 tcp signal");
    }
  },

  getUserHome: function() {
    return process.env[(process.platform == 'win32') ? 'USERPROFILE' : 'HOME'];
  },

  prepareAddSignalMessage: function(DataManager, projectId) {
    return new Promise(function(resolve, reject) {
      var _handleError = function(err) {
        console.log(err);
        reject(err);
      }

      var dataProvidersResult = DataManager.listDataProviders();
      var providers = [];
      dataProvidersResult.forEach(function(dataProvider) {
        providers.push(dataProvider.toObject())
      }) // end foreach dataProvidersResult

      // getting dataseries
      DataManager.listDataSeries().then(function(dataSeriesResult) {
        var series = [];
        dataSeriesResult.forEach(function(dataSeries) {
          series.push(dataSeries.toObject());
        }) // end foreach dataSeriesResult

        // getting collectors
        DataManager.listCollectors({}, projectId).then(function(collectorsResult) {
          var collectors = [];
          collectorsResult.forEach(function(collector) {
            // setting project id. temp. TODO: better way to implement it

            dataProvidersResult.some(function(dprovider) {
              return dataSeriesResult.some(function(dseries) {
                if (dprovider.id == dseries.data_provider_id && collector.input_data_series == dseries.id) {
                  //getting project id
                  collector.project_id = dprovider.project_id;
                  return true;
                }
              })
            });

            collectors.push(collector.toObject());
          }) // end foreach collectorsResult

          // getting analyses
          DataManager.listAnalyses().then(function(analysesResult) {
            var analyses = [];
            analysesResult.forEach(function(analysis) {
              analyses.push(analysis.toObject());
            }); // end foreach analysesResult

            resolve({
              "Analysis": analyses,
              "DataSeries": series,
              "DataProviders": providers,
              "Collectors": collectors
            });

          }).catch(_handleError); // end listAnalyses
        }).catch(_handleError); // end listCollectors
      }).catch(_handleError); // end listDataSeries
    });
  },

  findFiles: function(directory, pattern) {
    return new Promise(function(resolve, reject) {
      var fullPath = path.join(directory, pattern);

      glob(fullPath, function(err, files) {
        if (err)
          return reject(err);

        resolve(files);
      })
    })
  },

  listDynamicDataSeriesType: function() {
    var output = [];
    for(var key in Enums.DataSeriesType) {
      if (Enums.DataSeriesType.hasOwnProperty(key) && key != "STATIC_DATA") {
        var obj = {};
        obj["data_series_type_name"] = Enums.DataSeriesType[key];
        output.push(obj);
      }
    }

    return output;
  },

  matchObject: function(obj, target) {
    return Object.keys(obj).every(function(key) {
      return target[key] == obj[key];
    })
  },

  find: function(restriction, where) {
    return where.filter(function(entry) {
      return this.matchObject(restriction, entry)
    })
  },

  getServiceTypeName: function(intServiceType) {
    switch(intServiceType) {
      case Enums.ServiceType.COLLECTOR:
        return "COLLECTOR";
        break;
      case Enums.ServiceType.ANALYSIS:
        return "ANALYSIS";
        break;
      default:
        throw new exceptions.ServiceTypeError("Invalid service type value");
    }
  },

  isValidDataSeriesType: function(code) {
    switch(code) {
      case Enums.DataSeriesType.DCP:
      case Enums.DataSeriesType.STATIC_DATA:
      case Enums.DataSeriesType.OCCURRENCE:
      case Enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
      case Enums.DataSeriesType.GRID:
      case Enums.DataSeriesType.POSGIS:
        return true;
        break;
      default:
        throw new Error("Invalid data series type");
    }
  },

  getTimezonesGUI: function() {
    var output = [];
    for(var i = -12; i < 13; ++i) {
      var val = i < 0 ? i.toString() : "+" + i;
      output.push({
        name: val,
        value: val
      })
    }
    return output;
  },

  formatDateToTimezone: function(dateValue) {
    var timeZone = dateValue.getTimezoneOffset() / 60;
    var timezoneIndex = dateValue.toISOString().lastIndexOf("Z");
    var dateWithoutTimezone = dateValue.toISOString().slice(0, timezoneIndex);

    var tzStr;
    if (timeZone > 0) {
      tzStr = timeZone < 10 ? "-0" + timeZone + ":00" : "-" + timeZone + ":00";
    } else {
      var absTimezone = timeZone * (-1);
      tzStr = absTimezone < 10 ? "+0" + absTimezone + ":00" : "+" + absTimezone + ":00";
    }

    return dateWithoutTimezone + tzStr;
  },

  formatMetadataFromDB: function(values) {
    var metadata = {};
    values.forEach(function(meta) {
      metadata[meta.key] = meta.value;
    })

    return metadata;
  },

  sendDataToServices: function(DataManager, TcpManager, data) {
    DataManager.listServiceInstances().then(function(services) {
      services.forEach(function(service) {
        try {
          TcpManager.emit('sendData', service, data);
        } catch (e) {
          console.log(e);
        }
      });
    });
  },

  equal: function(origin, target) {
    return isEqual(origin, target);
  }
};
