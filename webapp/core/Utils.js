var Enums = require("./Enums");
var FormField = Enums.Form.Field;
var UriPattern = Enums.Uri;
var cloneDeep = require("lodash").cloneDeep;

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
  }
};