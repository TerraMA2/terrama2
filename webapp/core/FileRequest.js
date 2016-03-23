var AbstractRequest = require('./AbstractRequest');
var Promise = require('bluebird');
var fs = require('fs');

var FileRequest = function(params) {
  AbstractRequest.apply(this, arguments);
};

FileRequest.prototype = Object.create(AbstractRequest.prototype, {
  'constructor': FileRequest
});

FileRequest.prototype.request = function() {
  var self = this;
  return new Promise(function(resolve, reject) {
    fs.stat(self.params.path, function(err, stats) {
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

FileRequest.prototype.fields = function() {
  return {
    "name": "FILE",
    "path": "text"
  }
};


module.exports = FileRequest;