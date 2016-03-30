var Enums = require("./Enums");
var FormField = Enums.FormField;
var UriPattern = Enums.Uri;

module.exports = {
  clone: function(object) {
    return Object.assign({}, object);
  },

  handleRequestError: function(response, err, code) {
    response.status(code);
    response.json({status: code, message: err.message});
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
  }
};