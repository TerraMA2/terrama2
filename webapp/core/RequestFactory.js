var FtpRequest = require("./FtpRequest");
var HttpRequest = require("./HttpRequest");
var FileRequest = require("./FileRequest");
var PostgisRequest = require("./PostgisRequest");
var WcsRequest = require("./WcsRequest");
var ConnectionError = require("./Exceptions").ConnectionError;

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
          return new FileRequest(requestParameters);
          break;
        case "postgis":
          return new PostgisRequest(requestParameters);
        case "wcs":
          return new WcsRequest(requestParameters);
        default:
          throw new ConnectionError("Invalid request type");
      }
    }
    throw new ConnectionError("Request type not found");
  }
};


module.exports = RequestFactory;