'use strict';

// Dependencies
var AbstractRequest = require('./AbstractRequest');
var FtpRequest = require("./FtpRequest");
var SftpRequest = require("./SftpRequest");
var HttpRequest = require("./HttpRequest");
var HttpsRequest = require("./HttpsRequest");
var StaticHttpRequest = require("./StaticHttpRequest");
var FileRequest = require("./FileRequest");
var PostgisRequest = require("./PostgisRequest");
var WcsRequest = require("./WcsRequest");
var ConnectionError = require("./Exceptions").ConnectionError;
var PluginLoader = require('./PluginLoader');

/**
 * Request Helper: It prepares entire supported protocols e applies a switch to build the respective Request
 * @see {@link UriBuilder}
 * @throws {ConnectionError} When invalid protocol received.
 * @param {string} protocol - An URI protocol
 * @param {Object} requestParameters - A javascript object withing URI format.
 * @return {AbstractRequest} a request module
 */
function requestHelper(protocol, requestParameters) {
  switch (protocol) {
    case "ftp":
      return new FtpRequest(requestParameters);
    case "sftp":
      return new SftpRequest(requestParameters);
    case "http":
      return new HttpRequest(requestParameters);
    case "https":
      return new HttpsRequest(requestParameters);
    case "static-http":
      return new StaticHttpRequest(requestParameters);
    case "file":
      return new FileRequest(requestParameters);
    case "postgis":
      return new PostgisRequest(requestParameters);
    case "wcs":
      return new WcsRequest(requestParameters);
    default:
      // Check in all loaded plugins
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

/**
 * It defines a Request factory helpers
 * @type {Object}
 */
var RequestFactory = {
  /**
   * It builds a Request module from given URI object parameters
   * 
   * @throws {ConnectionError} When request type is not found or is not a string.
   * @param {Object} requestParameters - A javascript object within URI format
   * @return {AbstractRequest} A request module
   */
  build: function(requestParameters) {
    if (typeof requestParameters.protocol === "string") {
      var protocol = requestParameters.protocol.toLocaleLowerCase();
      return requestHelper(protocol, requestParameters);
    }
    throw new ConnectionError("Request type not found");
  },

  /**
   * It builds a Request module from given URI string
   *
   * @throws {ConnectionError} When request type is not found or is not a string.
   * @param {Object} requestParameters - A javascript object within URI format
   * @return {AbstractRequest} A request module 
   */
  buildFromUri: function(uri) {
    if (typeof uri === "string") {
      var protocol = uri.split("://")[0];
      return requestHelper(protocol, uri);
    }

    throw new ConnectionError("Request type not found");
  },

  /**
   * It retrieves entire supported request protocols fields
   * 
   * @todo add plugin support. Currently it is commented
   * @return {Array<Object>} An array of fields of each one request
   */
  listAll: function() {
    var array = [];

    // FtpFields
    array.push(FtpRequest.fields());

    // SftpFields
    array.push(SftpRequest.fields());

    // httpFields
    array.push(HttpRequest.fields());

    // httpsFields
    array.push(HttpsRequest.fields());

    array.push(StaticHttpRequest.fields());

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