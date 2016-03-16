var FtpRequest = require("./FtpRequest");
var HttpRequest = require("./HttpRequest");
var Promise = require('bluebird');

var RequestFactory = {
  build: function(requestParameters) {
    if (typeof requestParameters.kind === "string") {
      switch (requestParameters.kind.toLowerCase()) {
        case "ftp":
          return new FtpRequest(requestParameters);
          break;
        case "http":
          return new HttpRequest(requestParameters);
          break;
        case "file":
          throw new TypeError("Not supported yet");
          break;
        default:
          throw new TypeError("Invalid kind");
      }
    }
    throw new TypeError("Kind error");
  }
};


module.exports = RequestFactory;