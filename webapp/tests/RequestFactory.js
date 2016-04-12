var assert = require("assert");
var UriPattern = require("../core/Enums").Uri;


describe('RequestFactory', function() {
  var RequestFactory = require("../core/RequestFactory");

  it("should connect PostGIS server", function(done) {
    var params = {};
    params[UriPattern.SCHEME] = "POSTGIS";
    params[UriPattern.HOST] = "127.0.0.1";
    params[UriPattern.PORT] = 5432;
    params["database"] = "nodejs";
    params[UriPattern.USER] = "postgres";
    params[UriPattern.PASSWORD] = "postgres";

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect Http server", function(done) {
    var params = {};
    params[UriPattern.SCHEME] = "HTTP";
    params[UriPattern.HOST] = "www.inpe.br";
    params[UriPattern.PORT] = 80;
    params[UriPattern.PATHNAME] = "/";

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect local FILE", function(done) {
    var params = {};
    params[UriPattern.SCHEME] = "FILE";
    params[UriPattern.PATHNAME] = __dirname;

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect FTP server", function(done) {
    var params = {};
    params[UriPattern.SCHEME] = "FTP";
    params[UriPattern.HOST] = "ftp.cptec.inpe.br";
    params[UriPattern.PORT] = 21;
    params[UriPattern.PATHNAME] = "/";
    params[UriPattern.PASSWORD] = "eymar@dpi.inpe.br";
    params[UriPattern.USER] = "anonymous";

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect WCS server", function(done) {
    var params = {};
    params[UriPattern.SCHEME] = "WCS";
    params[UriPattern.HOST] = "sedac.ciesin.columbia.edu";
    params[UriPattern.PORT] = 80;
    params[UriPattern.PATHNAME] = "/geoserver/wcs";

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });
});