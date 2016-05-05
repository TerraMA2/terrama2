var Enums = require("./Enums");
var FormField = Enums.Form.Field;
var UriPattern = Enums.Uri;
var cloneDeep = require("lodash").cloneDeep;
var crypto = require('crypto');

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
  }
};