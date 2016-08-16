'use strict';

var AbstractRequest = require('./AbstractRequest');
var FtpRequest = require("./FtpRequest");
var HttpRequest = require("./HttpRequest");
var FileRequest = require("./FileRequest");
var PostgisRequest = require("./PostgisRequest");
var WcsRequest = require("./WcsRequest");
var ConnectionError = require("./Exceptions").ConnectionError;
var PluginLoader = require('./PluginLoader');


function requestHelper(protocol, requestParameters) {
  switch (protocol) {
    case "ftp":
      return new FtpRequest(requestParameters);
    case "http":
      return new HttpRequest(requestParameters);
    case "file":
      return new FileRequest(requestParameters);
    case "postgis":
      return new PostgisRequest(requestParameters);
    case "wcs":
      return new WcsRequest(requestParameters);
    default:
      for(var key in PluginLoader.plugin.plugins) {
        if (PluginLoader.plugin.plugins.hasOwnProperty(key)) {
          var klass = PluginLoader.plugin.plugins[key].object;

          if (klass && klass.prototype instanceof AbstractRequest) {
            if (klass.fields().name && klass.fields().name.toLowerCase() === protocol) {
              return new klass(requestParameters);
            }
          }
        }
      }

      throw new ConnectionError("Invalid request type");
  }
}


var RequestFactory = {
  build: function(requestParameters) {
    if (typeof requestParameters.protocol === "string") {
      var protocol = requestParameters.protocol.toLocaleLowerCase();
      return requestHelper(protocol, requestParameters);
    }
    throw new ConnectionError("Request type not found");
  },

  buildFromUri: function(uri) {
    if (typeof uri === "string") {
      var protocol = uri.split("://")[0];
      return requestHelper(protocol, uri);
    }

    throw new ConnectionError("Request type not found");
  },

  listAll: function() {
    var array = [];

    // FtpFields
    array.push(FtpRequest.fields());

    // httpFields
    array.push(HttpRequest.fields());

    // fileFields
    array.push(FileRequest.fields());

    // postgisFields
    array.push(PostgisRequest.fields());

    // for(var key in PluginLoader.plugin.plugins) {
    //   if (PluginLoader.plugin.plugins.hasOwnProperty(key)) {
    //     var klass = PluginLoader.plugin.plugins[key].object;

    //     if (klass && klass.prototype instanceof AbstractRequest) {
    //       if (klass.fields().name && klass.fields().name.toLowerCase() === scheme)
    //         array.push(klass.fields);
    //     }
    //   }
    // }

    return array;
  }
};


module.exports = RequestFactory;