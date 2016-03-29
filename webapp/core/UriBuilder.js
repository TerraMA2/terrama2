var Util = require('util');
var URL = require("url");

function buildUri(uriObjectGiven, uriSyntax) {
  var uriObject = {
    hostname: uriObjectGiven[uriSyntax.HOST],
    port: uriObjectGiven[uriSyntax.PORT],
    pathname: uriObjectGiven[uriSyntax.PATHNAME],
    slashes: true // It defines that URI protocol require colon-slash-slash
  };

  uriObject.protocol = uriObjectGiven[uriSyntax.SCHEME] ? uriObjectGiven[uriSyntax.SCHEME].toLowerCase() : null;

  if (uriObjectGiven[uriSyntax.AUTH])
    uriObject.auth = uriObjectGiven[uriSyntax.AUTH];
  else if (uriObjectGiven[uriSyntax.USER] || uriObjectGiven[uriSyntax.PASSWORD]) {
    uriObject.auth = Util.format("%s:%s", uriObjectGiven[uriSyntax.USER], uriObjectGiven[uriSyntax.PASSWORD]);
  }

  return URL.format(uriObject);
}

function buildObject(uriString, uriSyntax) {
  var uriObject = URL.parse(uriString);

  var auth;
  if (uriObject.auth)
    auth = uriObject.auth.split(':');

  var output = {};

  output[uriSyntax.HOST] = uriObject.protocol.substr(0, uriObject.protocol.length-1).toUpperCase();
  output[uriSyntax.PORT] = parseInt(uriObject.port);
  output[uriSyntax.USER] = auth ? auth[0] : '';
  output[uriSyntax.PASSWORD] = auth ? auth[1] : ''
  output[uriSyntax.PATHNAME] = uriObject.pathname;

  return output;
}

module.exports = {
  buildUri: buildUri,
  buildObject: buildObject
};