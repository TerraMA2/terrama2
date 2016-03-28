var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var fs = require('fs');
var FormField = require("./Enums").FormField;
var UriPattern = require("./Enums").Uri;
var Utils = require("./Utils");

var FileRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FileRequest.prototype = Object.create(AbstractRequest.prototype, {
  'constructor': FileRequest
});

FileRequest.prototype.request = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    fs.stat(self.params[self.syntax().PATHNAME], function(err, stats) {
      if (err && err.code === 'ENOENT') {
        return reject(new TypeError("Directory does not exists"));
      } else if (err) {
        return reject(new TypeError("Error in file request"));
      }

      if (stats.isFile() || stats.isDirectory())
        return resolve(true);
      else
        return reject(new TypeError("Error in file request"));
    });
  });
};

FileRequest.fields = function() {
  var properties = {};
  properties[UriPattern.PATHNAME] = {
    title: "Path",
    type: FormField.TEXT
  };
  return {
    "name": "FILE",
    properties: properties,
    required: [UriPattern.PATHNAME],
    display: [
      "*"
    ]
  }
};


module.exports = FileRequest;