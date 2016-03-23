var assert = require("assert");


describe('RequestFactory', function() {
  var RequestFactory = require("../core/RequestFactory");

  it("should connect PostGIS server", function(done) {
    var params = {
      address: "127.0.0.1",
      port: 5432,
      path: "/postgres",
      password: "postgres",
      user: "postgres",
      kind: "POSTGIS"
    };

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect Http server", function(done) {
    var params = {
      address: "www.inpe.br",
      port: 80,
      path: "/",
      password: "",
      user: "",
      kind: "HTTP"
    };

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect local FILE", function(done) {
    var params = {
      path: __dirname, // current path
      kind: "FILE"
    };

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect FTP server", function(done) {
    var params = {
      address: "ftp.cptec.inpe.br",
      port: 21,
      path: "/",
      password: "eymar@dpi.inpe.br",
      user: "anonymous",
      kind: "FTP"
    };

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it("should connect WCS server", function(done) {
    var params = {
      address: "flanche.net",
      port: 9090,
      path: "/rasdaman/",
      kind: "WCS"
    };

    RequestFactory.build(params).request().then(function() {
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });
});