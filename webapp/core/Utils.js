var Enums = require("./Enums");
var FormField = Enums.Form.Field;
var UriPattern = Enums.Uri;
var cloneDeep = require("lodash").cloneDeep;
var crypto = require('crypto');
var exceptions = require('./Exceptions');
var Signals = require('./Signals');

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
          return "Dcp";
          break;
        case Enums.AnalysisType.GRID:
          return "Grid";
          break;
        case Enums.AnalysisType.MONITORED:
          return "Monitored Object";
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
  }
};