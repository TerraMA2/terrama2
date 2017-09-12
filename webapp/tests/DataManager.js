var assert = require('assert');
var Enums = require('../core/Enums');

var DataSeriesType = Enums.DataSeriesType;

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
    data_provider_intent_name: "Collect"
  };
}

function createDataSeriesSemantic() {
  return {
    name: "Semantic 1",
    data_format_name: DataSeriesType.DCP,
    data_series_type_name: DataSeriesType.DCP
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

describe('DataManager', function() {
  var MainClass = require('../app');
  var app = require("../app");
  var config =  {
    "username": "postgres",
    "password": "postgres",
    "database": "nodejs_test",
    "host": "127.0.0.1",
    "dialect": "postgres",
    logging: false
  };

  var DataManager = require("../core/DataManager");
  DataManager.setConfiguration(config);

  // It runs before all tests. It initializes database, creating tables
  before(function(done){
    DataManager.init(null, function() {
      return done();
    });
  });

  // It runs after all tests. It drops each table.
  after(function(done){
    var DataManager = require("../core/DataManager");
    DataManager.connection.drop({cascade: true}).then(function(){
      return done();
    });
  });

  //describe("DataManager#init", function() {
  it('initializes DataManager database connection', function(done) {
    DataManager.init(null, function(){
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

    DataManager.addDataProvider(provider).then(function() {
      return done("Error: No exception thrown");
    }).catch(function() {
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

  it('should insert DataSeries with datasets', function(done) {
    var semantics = createDataSeriesSemantic();
    DataManager.addDataSeriesSemantics(semantics).then(function(semantic) {
      var dataSeries = createDataSeries();

      dataSeries.dataSets = [
        // dcp
        {
          id: 1,
          data_series_id: dataSeries.id,
          active: true,
          dataFormats: [
            {
              key: "Format1",
              value: "ValueFormat1"
            }
          ],
          position: {
            "type": "Point",
            "coordinates": [25.9, -42.78],
            "crs": {
              "type": "name",
              "properties": {
                "name": "EPSG:4326"
              }
            }
          }
        }
      ];

      DataManager.addDataSeries(dataSeries).then(function(result) {
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
    DataManager.getDataSet({id: 1}).then(function(dset) {
      console.log(dset);
      assert(dset.hasOwnProperty('position') && dset.position.coordinates[0] === 25.9);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should update DataSet', function(done) {
    var params = {id: 1, semantics: {
      data_series_type_name: "Dcp"
    }};
    var newPosition = {
      "type": "Point",
        "coordinates": [2, -2],
        "crs": {
        "type": "name",
          "properties": {
          "name": "EPSG:4326"
        }
      }
    };
    DataManager.updateDataSet(params, {active:false, position: newPosition}).then(function(result) {
      assert(result && result.position.coordinates[0] === 2);
      return done();
    }).catch(function(err) {
      return done(err);
    });
  });

  it('should list DataSets', function(done) {
    var dataSets = DataManager.listDataSets();
    return done(assert(dataSets.length === 1));
  });

  it('should update DataSeries', function(done) {
    DataManager.getDataSeries({name: "DataSeries1"}).then(function(result) {
      result.name = "Updated DataSeries1";

      DataManager.updateDataSerie(result).then(function() {
        assert(result.name === "Updated DataSeries1");
        return done();
      }).catch(function(err) {
        return done(err);
      });

    }).catch(function(err) {
      return done(err);
    });
  });

  it('should destroy DataSet', function(done) {
    DataManager.removeDataSet({id: 1}).then(function() {
      assert(DataManager.data.dataSets.length === 0);
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