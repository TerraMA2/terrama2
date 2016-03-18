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
    data_provider_type_name: "FTP",
    data_provider_intent_name: "Intent1"
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
    //define: {
    //  schema: "terrama2"
    //},
    logging: false
  };

  var DataManager = require("../core/DataManager");
  DataManager.setConfiguration(config);

  // It runs before all tests. It initializes database, creating tables
  before(function(done){
    DataManager.init(function() {
      return done();
    });
  });

  // It runs after all tests. It drops each table.
  after(function(done){
    var DataManager = require("../core/DataManager");
    DataManager.connection.drop({cascade: true}).then(function(e){
      console.log("Cleaning up database");

      return done();
    });
  });

  //describe("DataManager#init", function() {
  it('initializes DataManager database connection', function(done) {
    DataManager.init(function(){
      assert(DataManager.connection !== null);
      return done();
    });

  });

  it('loads data models to DataManager', function(done) {
    DataManager.load().then(function(){
      assert.notEqual(DataManager.data, {});
      return done();
    });
  });

  it('should insert Project in DataManager', function(done) {
    var project = createProject();

    DataManager.addProject(project).then(function(result) {
      assert(result.id > 0 && DataManager.data.projects.length == 1);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should insert DataProvider', function(done) {
    var provider = createDataProvider();

    DataManager.addDataProvider(provider).then(function(result) {
      assert(result.id > 0 && DataManager.data.dataProviders.length == 1);
      return done();
    }).catch(function(err) {
      return done(err);
    })
  });

  it('should not insert DataProvider', function(done) {
    var provider = createDataProvider();
    provider.data_provider_intent_id = 50;

    DataManager.addDataProvider(provider).then(function(result) {
      return done("Error: No exception thrown");
    }).catch(function(err) {
      return done();
    })
  });

  it('should retrieve a DataProvider', function(done){
    var expected = createDataProvider();

    DataManager.getDataProvider({name: expected.name}).then(function(provider) {
      assert(provider.name === expected.name);
      return done();
    }).catch(function(err) {
      return done(err);
    });

  });

  it('should update a DataProvider', function(done){
    var dataProvider = createDataProvider();
    dataProvider.name = "UpdatingProvider";

    DataManager.updateDataProvider(dataProvider).then(function(result) {
      assert(result.name === "UpdatingProvider");
      return done();
    }).catch(function(err) {
      return done(err);
    });

  });

  it('should insert DataSeries', function(done) {
    DataManager.addDataSeriesSemantics(createDataSeriesSemantic()).then(function(semantic) {
      var dataSeries = createDataSeries();

      dataSeries.dataSets = [
        {
          type: "dcp",
          id: 1,
          data_series_id: dataSeries.id,
          active: true,
          child: {
            id: 1,
            data_set_id: 1,
            position: {
              type: 'Point',
              coordinates: [39.807222,-76.984722],
              crs:{
                type: 'name',
                properties : {
                  name: 'EPSG:4326'}
              }
            },
            timeColumn: "timeColumn"
          },
          dataFormats: [
            {
              key: "Format1",
              value: "ValueFormat1"
            }
          ]
        },
        {
          type: "occurrence",
          id: 2,
          data_series_id: dataSeries.id,
          active: true,
          child: {
            id: 2,
            data_set_id: 2,
            geometryColumn: "geom_column",
            timeColumn: "time_field"
          }
        }
      ];

      DataManager.addDataSerie(dataSeries).then(function(result) {
        assert(result.id > 0 && DataManager.data.dataSeries.length == 1);
        return done();
      }).catch(function(err) {
        return done(err);
      });
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should retrieve DataSet', function(done) {
    DataManager.getDataSet({id: 1, type: "dcp"}).then(function(dset) {
      assert(dset.child.timeColumn === "timeColumn");
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should update DataSet', function(done) {
    var params = {id: 1, type: "dcp"};
    DataManager.getDataSet(params).then(function(dset) {
      DataManager.updateDataSet(params, {active:false, timeColumn: "TimeColumn3333"}).then(function(result) {
        assert(result.child.timeColumn === "TimeColumn3333");
        return done();
      }).catch(function(err) {
        return done(err);
      });
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should list DataSets', function(done) {
    var dataSets = DataManager.listDataSets();
    return done(assert(dataSets.length === 2));
  });

  it('should update DataSeries', function(done) {
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

  it('should destroy DataSet', function(done) {
    DataManager.removeDataSet({id: 1}).then(function() {
      assert(DataManager.data.dataSets.length === 1);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should destroy a DataSeries', function(done) {
    DataManager.removeDataSerie({name: "Updated DataSeries1"}).then(function() {
      assert(DataManager.data.dataSeries.length == 0);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should destroy a DataProvider', function(done){
    var expected = {
      id: 1
    };

    DataManager.removeDataProvider({id: expected.id}).then(function() {
      assert(DataManager.data.dataProviders.length === 0);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });
});