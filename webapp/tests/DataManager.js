var assert = require('assert');

function createProject() {
  return {
    id: 1,
    name: "Project 1",
    version: 1,
    description: "Test Project"
  };
}

function createDataProvider() {
  return {
    id: 1,
    name: "Provider 1",
    uri: "http://provider.com",
    description: "Test Provider",
    active: true,
    project_id: createProject().id,
    data_provider_type_id: 1,
    data_provider_intent_id: 1
  };
}

function createDataSeriesSemantic() {
  return {
    name: "Semantic 1",
    data_format_name: "Format 1",
    data_series_type_name: "DS Type 1"
  };
}

function createDataSeries() {
  return {
    id: 1,
    name: "DataSeries1",
    description: "Desc DataSeries1",
    data_series_semantic_name: createDataSeriesSemantic().name,
    data_provider_id: createDataProvider().id
  };
}

function createDataSet() {
  return {
    name: "DataSet1",
    active: true,
    data_series_id: createDataSeries().id
  }
}

describe('DataManager', function() {
  var MainClass = require('../app');
  var app = require("../app");
  var config =  {
    "username": "postgres",
    "password": "postgres",
    "database": "nodejs_test",
    "host": "127.0.0.1",
    "dialect": "postgres",
    //logging: false
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
      DataManager.load().then(function(){
        assert.notEqual(DataManager.data, {});
        return done();
      });
    });
  });

  it('should insert Project in DataManager', function(done) {
    DataManager.init(function(){
      DataManager.load().then(function(){
        var project = createProject();

        DataManager.addProject(project).then(function(result) {
          assert(result.id > 0 && DataManager.data.projects.length == 1);
          return done();
        }).catch(function(err) {
          return done(err);
        });
      });
    });
  });

  it('should insert DataProvider', function(done) {
    DataManager.init(function(){
      DataManager.load().then(function(){
        var provider = createDataProvider();

        DataManager.addDataProvider(provider).then(function(result) {
          assert(result.id > 0 && DataManager.data.dataProviders.length == 1);
          return done();
        }).catch(function(err) {
          return done(err);
        })
      });
    });
  });

  it('should not insert DataProvider', function(done) {
    DataManager.init(function(){
      DataManager.load().then(function(){
        var provider = createDataProvider();
        provider.data_provider_intent_id = 50;

        DataManager.addDataProvider(provider).then(function(result) {
          return done("Error: No exception thrown");
        }).catch(function(err) {
          return done();
        })
      });
    });
  });

  it('should retrieve a DataProvider', function(done){
    DataManager.init(function(){
      DataManager.load().then(function(){
        var expected = createDataProvider();

        DataManager.getDataProvider({name: expected.name}).then(function(provider) {

          assert.equal(provider.name, expected.name);
          return done();

        }).catch(function(err) {

          return done(err);

        });
      });
    });
  });

  it('should update a DataProvider', function(done){
    DataManager.init(function(){
      DataManager.load().then(function(){
        var dataProvider = {
          name: "Provider 1"
        };
        DataManager.getDataProvider({name: dataProvider.name}).then(function(provider) {
          provider.name = "UpdatingProvider";

          DataManager.updateDataProvider(provider).then(function(result) {
            assert.equal(result.name, "UpdatingProvider");
            return done();
          }).catch(function(err) {
            return done(err);
          });
        });
      });
    });
  });

  it('should insert DataSeries', function(done) {
    DataManager.init(function() {
      DataManager.load().then(function() {

        DataManager.addDataSeriesSemantic(createDataSeriesSemantic()).then(function(semantic) {
          DataManager.addDataSerie(createDataSeries()).then(function(result) {
            assert(result.id > 0 && DataManager.data.dataSeries.length == 1);
            return done();
          }).catch(function(err) {
            return done(err);
          });
        }).catch(function(err) {
          return done(err);
        });

      });
    });
  });

  it('should update DataSeries', function(done) {
    DataManager.init(function() {
      DataManager.load().then(function() {

        DataManager.getDataSerie({name: "DataSeries1"}).then(function(result) {
          result.name = "Updated DataSeries1";

          DataManager.updateDataSerie(result).then(function() {
            assert(result.name === "Updated DataSeries1");
            return done();
          }).catch(function(err) {
            return done(err);
          });

        });

      });
    });
  });

  it('should destroy a DataSeries', function(done) {
    DataManager.init(function() {
      DataManager.load().then(function() {

        DataManager.removeDataSerie({name: "Updated DataSeries1"}).then(function() {
          assert(DataManager.data.dataSeries.length == 0);
          return done();
        }).catch(function(err) {
          return done(err);
        });

      });
    });
  });

  it('should destroy a DataProvider', function(done){
    DataManager.init(function(){
      DataManager.load().then(function(){
        var expected = {
          name: "UpdatingProvider"
        };

        DataManager.removeDataProvider({name: expected.name}).then(function() {
          assert.equal(DataManager.data.dataProviders.length, 0);
          return done();
        }).catch(function(err) {
          return done(err);
        });
      });
    });
  });
});