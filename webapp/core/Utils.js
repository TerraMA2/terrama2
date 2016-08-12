"use strict";

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

var Utils = {
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
    }];
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
    };
  },

  rollbackPromises: function(promises, exception, errorHandler) {
    Promise.all(promises).then(function() {
      errorHandler(exception);
    }).catch(function(err) {
      errorHandler(err);
    });
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
      var output = null;
      switch(parseInt(analysisCode)) {
        case Enums.AnalysisType.DCP:
          output = Enums.DataSeriesType.DCP;
          break;
        case Enums.AnalysisType.GRID:
          output = Enums.DataSeriesType.GRID;
          break;
        case Enums.AnalysisType.MONITORED:
          output = Enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT;
          break;
        default:
          output = null;
      }

      if (output && output !== null) { return output; }
    }
    throw new Error("Invalid analysis id");
  },

  getTcpSignal: function(value) {
    switch(value) {
      case Signals.TERMINATE_SERVICE_SIGNAL:
      case Signals.STATUS_SIGNAL:
      case Signals.ADD_DATA_SIGNAL:
      case Signals.START_PROCESS_SIGNAL:
      case Signals.LOG_SIGNAL:
      case Signals.REMOVE_DATA_SIGNAL:
      case Signals.PROCESS_FINISHED_SIGNAL:
      case Signals.UPDATE_SERVICE_SIGNAL:
        return value;
      default:
        throw new exceptions.SignalError("Invalid terrama2 tcp signal");
    }
  },

  getUserHome: function() {
    return process.env[(process.platform === 'win32') ? 'USERPROFILE' : 'HOME'];
  },

  prepareAddSignalMessage: function(DataManager, projectId) {
    return new Promise(function(resolve, reject) {
      var _handleError = function(err) {
        console.log(err);
        reject(err);
      };

      var dataProvidersResult = DataManager.listDataProviders();
      var providers = [];
      dataProvidersResult.forEach(function(dataProvider) {
        providers.push(dataProvider.toObject());
      }); // end foreach dataProvidersResult

      // getting dataseries
      DataManager.listDataSeries().then(function(dataSeriesResult) {
        var series = [];
        dataSeriesResult.forEach(function(dataSeries) {
          series.push(dataSeries.toObject());
        }); // end foreach dataSeriesResult

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
              });
            });

            collectors.push(collector.toObject());
          }); // end foreach collectorsResult

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
        if (err) { return reject(err); }

        resolve(files);
      });
    });
  },

  listDynamicDataSeriesType: function() {
    var output = [];
    for(var key in Enums.DataSeriesType) {
      if (Enums.DataSeriesType.hasOwnProperty(key) && key !== "STATIC_DATA") {
        var obj = {};
        obj.data_series_type_name = Enums.DataSeriesType[key];
        output.push(obj);
      }
    }

    return output;
  },

  /**
   * A deep match object. It checks every key/object in target and match them from initial object.
   * It applies a auto recursive call when obj key is pointing to an another object.
   * @todo Compares with equality operator (===). Currently, it is working with == operator
   * @todo Handle invalid use of Enums.Operators in validation
   * @param {Object} obj - An javascript object with key/values to check.
   * @param {Object} target - An javascript object to be watched
   * @return {Boolean} a boolean condition of comparator.
   */
  matchObject: function(obj, target) {
    var self = this;
    return Object.keys(obj).every(function(key) {
      if (self.isObject(obj[key])) {
        return self.matchObject(obj[key], target[key]);
      }

      switch (key) {
        case Enums.Operators.EQUAL:
          return target === obj[key];
        case Enums.Operators.GREATER_THAN:
          return target > obj[key];
        case Enums.Operators.GREATER_OR_EQUAL:
          return target >= obj[key];
        case Enums.Operators.LESS_THAN:
          return target < obj[key];
        case Enums.Operators.LESS_EQUAL:
          return target <= obj[key];
        case Enums.Operators.NOT_EQUAL:
          return target !== obj[key];
        default:
          // equal operator
          return target[key] == obj[key];
      }
    });
  },
  /**
   * It applies a deep filter in array from restriction
   * @param {Array<?>} where - An array of any to be filtered
   * @param {Object} restriction - A javascript object with restriction values
   * @return {Array<?>} a filtered array
   */
  filter: function(where, restriction) {
    var self = this;
    return where.filter(function(entry) {
      return self.matchObject(restriction, entry);
    });
  },
  /**
   * It performs a deep find in array from restriction. Note if more than one has found, it will get first element.
   * @param {Array<?>} where - An array of any to be filtered
   * @param {Object} restriction - A javascript object with restriction values
   * @return {?} An element of array.
   */
  find: function(where, restriction) {
    return this.filter(where, restriction)[0];
  },

  /**
   * It performs a remove operation of array from given restriction.
   * @param {Array<?>} where - An array of object to be iterated
   * @param {Object} restriction - A javascript object with restriction values
   * @return {?} An element of array.
   *
   * @example
   * - Remove a "foo" element
   * var elementRemoved = Utils.remove(array, {name: "foo"});
   */
  remove: function(where, restriction) {
    var self = this;
    var index = -1;

    var element = where.find(function(entry, entryIndex) {
      index = entryIndex;
      return self.matchObject(restriction, entry);
    });

    if (element) {
      where.splice(index, 1);
    }

    return element;
  },

  /**
   * It performs a remove operation of all occurrences in array from given restriction.
   * @param {Array<?>} where - An array of object to be iterated
   * @param {Object} restriction - A javascript object with restriction values
   * @return {Array<?>} Array of affected elements.
   *
   * @example
   * - Remove all "foo" occurrences
   * var elementsRemoved = Utils.remove(array, {name: "foo"});
   */
  removeAll: function(where, restriction) {
    var self = this;
    var indexes = [];

    var elements = where.filter(function(entry, entryIndex) {
      var matched = self.matchObject(restriction, entry);
      if (matched) {
        indexes.push(entryIndex);
      }
      return matched;
    });

    while(indexes.length > 0) {
      where.splice(indexes.pop(), 1);
    }

    return elements;
  },

  getServiceTypeName: function(intServiceType) {
    var output = null;
    switch(intServiceType) {
      case Enums.ServiceType.COLLECTOR:
        output = "COLLECTOR";
        break;
      case Enums.ServiceType.ANALYSIS:
        output = "ANALYSIS";
        break;
    }

    if (output && output !== null) { return output; }

    throw new exceptions.ServiceTypeError("Invalid service type value");
  },

  isValidDataSeriesType: function(code) {
    switch(code) {
      case Enums.DataSeriesType.DCP:
      case Enums.DataSeriesType.STATIC_DATA:
      case Enums.DataSeriesType.OCCURRENCE:
      case Enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
      case Enums.DataSeriesType.GRID:
      case Enums.DataSeriesType.POSTGIS:
        return true;
      default:
        throw new Error("Invalid data series type");
    }
  },

  getTimezonesGUI: function() {
    var output = [];
    for(var i = -12; i < 13; ++i) {
      var val = i < 0 ? i.toString() : "+" + i;
      output.push({ name: val, value: val });
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
    });

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
  },

  /**
   * It checks if a argument is instance of javascript Object
   *
   * @param {?} arg - A value
   * @return {Boolean} A boolean result
   */
  isObject: function(arg) {
    return arg === Object(arg);
  },

  /**
   * It creates a copy of object.
   * @param {Object} object - a object to be copied
   * @param {DataModel} model - a TerraMA2 model (optional)
   */
  makeCopy: function(object, model) {
    if (model) {
      return new model(object);
    } else {
      return Object.assign({}, object);
    }
  }
};

module.exports = Utils;