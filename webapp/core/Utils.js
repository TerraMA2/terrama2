"use strict";

var Enums = require("./Enums");
var FormField = Enums.Form.Field;
var UriPattern = Enums.Uri;
var _ = require("lodash");
var cloneDeep = _.cloneDeep;
var crypto = require('crypto');
var exceptions = require('./Exceptions');
var Signals = require('./Signals');
var Promise = require("bluebird");
var util = require('util');
var isEqual = require('lodash').isEqual;
/**
 * Encrypt/Decrypt module
 * @type {bcrypt}
 */
var bcrypt = require("bcrypt");
/**
 * TerraMA² URI Builder.
 * @type {URIBuilder}
 */
var URIBuilder = require("./../core/UriBuilder");

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

var Utils = module.exports = {
  clone: function(object) {
    return cloneDeep(object);
  },

  handleRequestError: function(response, err, code) {
    var message = err instanceof exceptions.BaseError ? err.toStr() : err.message;
    var errors = err instanceof exceptions.ValidationError ? err.getErrors() : {};
    response.status(code);
    return response.json({status: code, message: message, errors: errors});
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

  generateToken: function(app, code, intent, extra) {
    var token = crypto.randomBytes(48).toString('hex');
    app.locals.tokenIntent = {
      token: token,
      code: code,
      intent: intent
    };
    if (extra){
      app.locals.tokenIntent.extra = extra;
    }

    return token;
  },

  getTokenCodeMessage: getTokenCodeMessage,

  makeTokenParameters: function(token, app) {
    var parameters = {};

    if (app.locals.tokenIntent && app.locals.tokenIntent.token === token) {
      var code = app.locals.tokenIntent.code;
      var intent = app.locals.tokenIntent.intent;

      parameters.message = intent + " " + getTokenCodeMessage(code);
      if (app.locals.tokenIntent.extra){
        parameters.extra = app.locals.tokenIntent.extra;
      }
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

  /**
   * It retrieves a TCP signal from given value
   *
   * @param {number} value - A tcp signal number
   * @returns {Signals}
   */
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

  /**
   * It retrieves a user home directory (win32/linux/mac)
   *
   * @returns {string}
   */
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
                if (dprovider.id == dseries.data_provider_id && collector.data_series_input == dseries.id) {
                  //getting project id
                  collector.project_id = dprovider.project_id;
                  return true;
                }
              });
            });

            collectors.push(collector.toObject());
          }); // end foreach collectorsResult

          // getting analysis
          DataManager.listAnalysis().then(function(analysisResult) {
            var analysisArr = [];

            analysisResult.forEach(function(analysis) {
              analysisArr.push(analysis.toObject());
            }); // end foreach analysisResult

            DataManager.listViews().then(function(views) {
              var viewsArr = [];
              views.forEach(function(view) {
                viewsArr.push(view.toObject());
              });

              return resolve({
                "Analysis": analysisArr,
                "DataSeries": series,
                "DataProviders": providers,
                "Collectors": collectors,
                "Views": viewsArr
              });
            }).catch(_handleError); // end views
          }).catch(_handleError); // end listAnalysis
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
        switch(key) {
          case Enums.Operators.IN: // $in: [a,b,c...d]
            return obj[key].indexOf(target) !== -1;
          default:
            if (target instanceof Array) {
              return self.filter(target, obj);
            } else {
              return self.matchObject(obj[key], target[key]);
            }
        }
      }

      if (target instanceof Array) {
        return self.find(target, obj);
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
    var self = this;
    return where.find(function(entry) {
      return self.matchObject(restriction, entry);
    });
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
  /**
   * It retrieves a Service Type Name from identifier.
   *
   * @throws {exceptions.ServiceTypeError} When service type is invalid
   * @param {number} intServiceType - A TerraMA² service type identifier
   * @returns {string}
   *
   * @example
   * $ console.log(Utils.getServiceTypeName(Enums.ServiceType.COLLECTOR))
   * // output -> "COLLECTOR"
   */
  getServiceTypeName: function(intServiceType) {
    var output = null;
    switch(intServiceType) {
      case Enums.ServiceType.COLLECTOR:
        output = "COLLECTOR";
        break;
      case Enums.ServiceType.ANALYSIS:
        output = "ANALYSIS";
        break;
      case Enums.ServiceType.VIEW:
        output = "VIEW";
        break;
    }

    if (output && output !== null) { return output; }

    throw new exceptions.ServiceTypeError("Invalid service type value");
  },

  isValidDataSeriesType: function(code) {
    switch(code) {
      case Enums.DataSeriesType.DCP:
      case Enums.DataSeriesType.GEOMETRIC_OBJECT:
      case Enums.DataSeriesType.OCCURRENCE:
      case Enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
      case Enums.DataSeriesType.GRID:
      case Enums.DataSeriesType.POSTGIS:
        return true;
      default:
        throw new Error("Invalid data series type");
    }
  },

  /**
   * It retrieves a list of timezones to fill out elements in GUI interface.
   *
   * @returns {string[]}
   */
  getTimezonesGUI: function() {
    var output = [];
    for(var i = -12; i < 13; ++i) {
      var val = i < 0 ? i.toString() : ((i === 0) ? i.toString() : "+" + i);
      output.push({ name: val, value: val });
    }
    return output;
  },

  formatDateToTimezone: function(dateValue) {
    var timeZone = dateValue.getTimezoneOffset() / 60;
    var dateOffSetTimezone = new Date(dateValue.getTime() - (dateValue.getTimezoneOffset() * 60000));
    var timezoneIndex = dateValue.toISOString().lastIndexOf("Z");
    var dateWithoutTimezone = dateOffSetTimezone.toISOString().slice(0, timezoneIndex);

    var tzStr;
    if (timeZone > 0) {
      tzStr = timeZone < 10 ? "-0" + timeZone + ":00" : "-" + timeZone + ":00";
    } else {
      var absTimezone = timeZone * (-1);
      tzStr = absTimezone < 10 ? "+0" + absTimezone + ":00" : "+" + absTimezone + ":00";
    }

    return dateWithoutTimezone + tzStr;
  },

  /**
   * It performs a data creation from given date string terrama2 format
   *
   * @example
   * "2016-09-05T08:00:00.000-03:00" => Date
   * @throws {Error} When a date is not a string
   * @param {string} dateString - A TerraMA² date format string
   * @returns {Date}
   */
  dateFromFormat: function(dateString) {
    if (this.isString(dateString)) {
      var stringLen = dateString.length;
      var timezone = parseInt(dateString.substr(stringLen - 6, stringLen - 3));
      var dateWithoutTimezone = dateString.substr(0, stringLen - 6);
      var date = new Date(dateWithoutTimezone);

      date.setUTCHours(date.getUTCHours() - (timezone));
      return date;
    }
    throw new Error("Date must be a string");
  },

  /**
   * It formats a database metadata (key, value) to a javascript object.
   *
   * @example
   * var dbMetadata = [
   *   {id: 1, key: "foo", value: "bar"},
   *   {id: 2, key: "foo2", value: "bar2"},
   * ]
   *
   * Utils.formatMetadataFromDB(dbMetadata) -> {"foo": "bar", "foo2": "bar2"}
   * @param {Array<String>} values - An array of metadata values
   * @param {Function} fn - A function to perform each iteration
   * @returns {Object}
   */
  formatMetadataFromDB: function(values, fn) {
    var metadata = {};

    if (this.isFunction(fn)) {
      values.forEach(function(meta) {
        metadata[meta.key] = fn(meta.value);
      });
    } else {
      values.forEach(function(meta) {
        metadata[meta.key] = meta.value;
      });
    }

    return metadata;
  },
  /**
   * It generates an Array from object iteration, calling a callback to build result
   *
   * @param {Object} valuesObject - A javascript object to iterate
   * @param {Function} operationIter - A callback to be called in object iteration. It should return something (object)
   * @param {any} extra - An extra values to iterate. It will be passed through function iteration
   * @returns {Array<?>}
   */
  generateArrayFromObject: function(valuesObject, operationIter, extra) {
    var metadataArr = [];
    for(var k in valuesObject) {
      if (valuesObject.hasOwnProperty(k)) {
        metadataArr.push(operationIter(k, valuesObject[k], extra));
      }
    }
    return metadataArr;
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
  /**
   * It performs a REMOVE_DATA_SIGNAL to TCP Services
   *
   * @param {DataManager} DataManager - A TerraMA² DataManager instance injected as dependency.
   * @param {TcpManager} TcpManager - A TerraMA² TcpManager instance .
   * @param {Object} data - A data to remove
   * @param {number[]} data.Views - A list of view ids to remove
   */
  removeDataSignal: function(DataManager, TcpManager, data, serviceRestriction) {
    DataManager.listServiceInstances(serviceRestriction).then(function(services) {
      services.forEach(function(service) {
        try {
          TcpManager.emit('removeData', service, data);
        } catch (e) {
          console.log(e);
        }
      });
    });
  },

  /**
   * It generates a encrypted URI
   *
   * @param {string} uri - An URI to encrypt
   * @param {Enums.Uri?} syntax - An URI syntax. Default is Enums.Uri
   * @param {string} salt - A salt to create encrypted URI
   * @returns {string}
   */
  encryptURI: function(uri, syntax, salt) {
    // Setting default syntax
    if (!syntax) {
      syntax = Enums.Uri;
    }

    if (Utils.isString(uri)) {
      /**
       * It will stores a URI Object syntax
       * @type {Object}
       */
      var uriObject = URIBuilder.buildObject(uri, syntax);
      /**
       * URI user encrypted
       * @type {string}
       */
      var passwd = bcrypt.hashSync(uriObject[syntax.PASSWORD], salt);

      uriObject[syntax.PASSWORD] = passwd;

      // returns a built uri string
      return URIBuilder.buildUri(uriObject, syntax);
    }

    throw new Error("Invalid URI to encrypt");
  },

  /**
   * It generates a Salt value
   * @param {number} rounds - It represents the cost of processing the data. Default 10.
   * @returns {string}
   */
  generateSalt: function(rounds) {
    if (!this.isNumber(rounds)) {
      rounds = 10;
    }
    return bcrypt.genSaltSync(rounds);
  },

  /**
   * It compares URI encrypted with another URI
   *
   * @param {string} uri - An URI to encrypt
   * @param {string} encryptedURI - An encrypted URI
   * @returns {Boolean}
   */
  compareURI: function(uri, encryptedURI) {
    return bcrypt.compareSync(uri, encryptedURI);
  },

  /**
   * It compares two objects if they are same (including attributes).
   *
   * @param {Object} origin
   * @param {Object} target
   * @return {boolean} A boolean comparison
   */
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
   * It checks if a argument is a number and a finite number.
   *
   * @param {?} arg - A value
   * @returns {Boolean}
   */
  isNumber: function(arg) {
    return _.isFinite(arg);
  },

  /**
   * It checks if a argument is a function
   *
   * @param {?} arg - A value
   * @return {Boolean} A boolean result
   */
  isFunction: function(arg) {
    return typeof arg === "function";
  },

  /**
   * It checks if a argument is instance of javascript string
   *
   * @param {?} arg - A value
   * @return {Boolean} A boolean result
   */
  isString: function(arg) {
    return typeof(arg) == 'string' || arg instanceof String;
  },

  /**
   * It checks if given parameter is empty.
   *
   * @param {Array | Object}
   */
  isEmpty: function(arg) {
    if (!arg) {
      return true;
    }
    if (arg instanceof Array) {
      return arg.length === 0;
    }
    return Object.keys(arg).length === 0;
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
  },

  /**
   * It performs a extend object from given parameters
   *
   * @param {Object} nodeA - A first node
   * @param {Object} nodeB - A second node
   * @return {Object} An extended object with A and B
   */
  extend: function(nodeA, nodeB) {
    return _.extend(nodeA || {}, nodeB || {});
  },
  /**
   * It applies a string format over arguments.
   * @param {...string|number} args A list of arguments.
   * @returns {string}
   *
   * @example
   * > Utils.format("Hi %s", "User")  // "Hi User"
   * > Utils.format(1, 2, 3)  // "1 2 3"
   */
  format: function() {
    return util.format.apply(this, arguments);
  },

  /**
   * It performs a array concat with arguments values.
   *
   * @param {any[]} array - Array of elements to concatenate
   * @param {...any} [values] - The values to concatenate
   */
  concat: function() {
    return _.concat.apply(this, arguments);
  },

  /**
   * It just builds a folder object used in schema form redraw
   *
   * @returns {Object}
   */
  getFolderSchema: function() {
    return {
      folder: {
        "type": Enums.Form.Field.TEXT,
        "title": "Folder",
        "x-schema-form": {
          "pattern": "[^:!&'()*+,;=\[\]{}]*"
        }
      }
    };
  },

  /**
   * It just builds a folder object form representation for schema form
   *
   * @returns {Object}
   */
  getFolderForm: function() {
    return {
      key: "folder",
      htmlClass: "col-md-6 col-sm-12 terrama2-schema-form"
    };
  }
};
