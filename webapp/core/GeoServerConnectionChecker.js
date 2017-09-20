"use strict"

/**
 * Class responsible to check connection with GeoServer
 * @class GeoServerConnectionChecker
 * 
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 * 
 * @property {object} json - Object with param to check connection.
 * @property {object} callback - Function to callback the result.
 * @property {object} memberRequest - 'request' module.
 */
var GeoServerConnectionChecker = function(json, callback){

  // Request class
  var memberRequest = require('request');

  var returnObject = {
    error: false,
    message: ""
  };

  var protocol = "http://";

  if(json.host.indexOf("http://") !== -1) {
    var host = json.host.replace("http://", "");
  } else if(json.host.indexOf("https://") !== -1) {
    var host = json.host.replace("https://", "");
    var protocol = "https://";
  } else {
    var host = json.host;
  }

  var hostArray = host.split("/");
  host = hostArray.shift();
  var path = hostArray.join("/");

  if(path.charAt(path.length - 1) == "/")
    path = path.slice(0, -1);

  if(path.substr(path.length - 4) === "/web")
    path = path.replace("/web", "");

  if(path.substr(path.length - 4) === "/ows")
    path = path.replace("/ows", "");

  var address = protocol + host + ":" + json.port + (path != "" ? "/" + path : "") + "/rest/layers";

  var options = {
    url: address,
    method: 'GET',
    auth: {
      user: json.username,
      pass: json.password
    },
    headers: {
      'accept': 'application/json',
      'content-Type': 'application/json'
    }
  };

  memberRequest(options, function(err, res, html) {
    if(err) {
      returnObject.error = true;

      switch(err.errno) {
        case "ENOTFOUND":
          returnObject.message = "Address not found";
          break;
        case "ETIMEDOUT":
          returnObject.message = "Connection timeout, verify the port";
          break;
        case "EPROTO":
          returnObject.message = "Invalid port";
          break;
        case "ECONNREFUSED":
          returnObject.message = "The connection was refused by the server, verify the connection parameters";
          break;
        default:
          returnObject.message = "Failed to connect, verify the connection parameters";
      }
    } else {
      if(res.statusCode == 401 || html.toLowerCase().indexOf("http status 401") !== -1) {
        returnObject.error = true;
        returnObject.message = "Username or password does not match";
      } else if(res.statusCode == 404 || html.toLowerCase().indexOf("404 not found") !== -1 || html.toLowerCase().indexOf("http status 404") !== -1) {
        returnObject.error = true;
        returnObject.message = "Address not found";
      } else if(res.statusCode != 200) {
        returnObject.error = true;
        returnObject.message = "Failed to connect, verify the connection parameters";
      } else {
        try {
          var returnedData = JSON.parse(html);

          //if(returnedData.layers.layer.length >= 0) {
            returnObject.error = false;
            returnObject.message = "Success";
          //} else {
            //returnObject.error = true;
            //returnObject.message = "Failed to connect, verify the connection parameters";
          //}
        } catch(err) {
          returnObject.error = true;
          returnObject.message = "Failed to connect, verify the connection parameters";
        }
      }
    }

    returnObject.message += ". Important! Make sure the server's REST API is enabled";

    callback(returnObject);
  });
};

module.exports = GeoServerConnectionChecker;