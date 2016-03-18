var Util = require('util');
var URL = require("url");


function buildUri(uriObjectGiven) {
  var uriObject = {
    hostname: uriObjectGiven.host || uriObjectGiven.address,
    port: uriObjectGiven.port,
    protocol: uriObjectGiven.kind.toLowerCase() || uriObjectGiven.scheme.toLowerCase(),
    pathname: uriObjectGiven.path || uriObjectGiven.pathname
  };

  if (uriObjectGiven.auth)
    uriObject.auth = uriObjectGiven.auth;
  else if (uriObjectGiven.user || uriObjectGiven.username || uriObjectGiven.pass || uriObjectGiven.password) {
    var user = uriObjectGiven.user || uriObjectGiven.username;
    var password = uriObjectGiven.pass || uriObjectGiven.password;
    uriObject.auth = Util.format("%s:%s", user ? user : "", password ? password : "");
  }

  return URL.format(uriObject);
}

function buildObject(uriString) {
  // todo: parse
  return URL.parse(uriString);
}

module.exports = {
  buildUri: buildUri,
  buildObject: buildObject
};