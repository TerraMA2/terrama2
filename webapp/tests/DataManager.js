var assert = require('assert');

describe('DataManager', function() {
  var MainClass = require('../app');
  var app = require("../app");
  var config =  {
    "username": "postgres",
    "password": "postgres",
    "database": "nodejs",
    "host": "127.0.0.1",
    "dialect": "postgres",
    logging: false
  };
  app.set("databaseConfig", config);
  var DataManager = require("../core/DataManager");

  // It runs before all tests. It initializes database, creating tables
  before(function(done){
    DataManager.init(function() {
      done();
    });
  });

  // It runs after all tests. It drops each table.
  after(function(done){
    var DataManager = require("../core/DataManager");
    DataManager.connection.drop({cascade: true}).then(function(e){
      console.log("Cleaning up database");

      done();
    });
  });

  //describe("DataManager#init", function() {
  it('initializes DataManager database connection', function(done) {
    DataManager.init(function(){
      assert.notEqual(DataManager.connection, null);
      return done();
    });

  });

  it('loads data models to DataManager', function(done) {
    DataManager.init(function(){
      DataManager.load(function(){
        assert.notEqual(DataManager.data, {});
        return done();
      });
    });
  });

  it('should insert Project in DataManager', function(done) {
    DataManager.init(function(){
      DataManager.load(function(){
        var project = {
          id: 1,
          name: "Project 1",
          version: 1,
          description: "Test Project"
        };

        DataManager.addProject(project, function(result){
          assert(result.id > 0 && DataManager.data.projects.length == 1);
          return done();
        });
      });
    });
  });

  it('should insert DataProvider in DataManager', function(done) {
    DataManager.init(function(){
      DataManager.load(function(){
        var provider = {
          name: "Provider 1",
          uri: "http://provider.com",
          description: "Test Provider",
          active: true,
          project_id: 1,
          data_provider_type_id: 1,
          data_provider_intent_id: 1
        };

        DataManager.addDataProvider(provider, function(result){
          assert(result.id > 0 && DataManager.data.dataProviders.length == 1);
          return done();
        });
      });
    });
  });

  it('should retrieve a DataProvider', function(done){
    DataManager.init(function(){
      DataManager.load(function(){
        var expected = {
          name: "Provider 1",
          uri: "http://provider.com",
          description: "Test Provider",
          active: true,
          project_id: 1
        };

        var provider = DataManager.getDataProvider({name: expected.name});

        assert.equal(provider.name, expected.name);

        return done();
      });
    });
  });

  it('should destroy a DataProvider', function(done){
    DataManager.init(function(){
      DataManager.load(function(){
        var expected = {
          name: "Provider 1",
          uri: "http://provider.com",
          description: "Test Provider",
          active: true,
          project_id: 1
        };

        DataManager.removeDataProvider({name: expected.name}, function() {
          assert.equal(DataManager.data.dataProviders.length, 0);

          return done();
        });
      });
    });
  });
});