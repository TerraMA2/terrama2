var tcpManager = require("./TcpManager")();
var SIGNALS = require("./Signals");
var modelsFn = require("../models");


// Helpers
Array.prototype.removeItem = function (key) {
  var tmp = this.splice(key, 1);
  if (tmp.length == 1)
    return tmp[0];
  return null;
};

Array.prototype.getItemByParam = function(object) {
  var key = Object.keys(object)[0];
  return this.find(function(item){
    return (item[key] == object[key]);
  });
};

var models = null;
var actualConfig = {};

/*
 It must be set in sequelize model hooks to make it synchronized.
 todo: Should it has timer for reload caching object?
*/
var DataManager = {
  data: {
    dataSeries: [],
    dataProviders: [],
    projects: []
  },
  connection: null,

  init: function(callback) {
    var app = require('../app');
    var Sequelize = require("sequelize");
    var databaseConfig = app.get("databaseConfig");

    if (!Object.is(databaseConfig, actualConfig)) {
      actualConfig = databaseConfig;

      var connection = new Sequelize(actualConfig.database,
        actualConfig.username,
        actualConfig.password,
        actualConfig);

      models = modelsFn();
      models.load(connection);

      this.connection = connection;

      var fn = function() {
        // todo: insert default values in database
        models.db.DataProviderType.create({name: "Type1", description: "Desc Type1"}).then(function(result){
          models.db.DataProviderIntent.create({name: "Intent1", description: "Desc Intent2"}).then(function(result){
            callback();
            return;
          }).catch(function(e){
            callback();
          });
        }).catch(function(e){
          callback();
        });
      };

      connection.sync().then(function () {
        fn();
      }, function(err) {
        fn();
      });
    }
    else
      callback();
  },

  load: function(callback) {
    var self = this;

    models.db.DataProvider.findAll({}).then(function(dataProviders){
      self.data.dataProviders = dataProviders;
      return callback();
    });
  },

  addProject: function(projectObject, callback) {
    if (this.connection)
    {
      var self = this;
      models.db.Project.create(projectObject).then(function(project){
        self.data.projects.push(project);
        callback(project);
      });
    }
    else
      throw TypeError("DataManager not initialized yet. Could not save Project");
  },

  addDataProvider: function(dataProviderObject, callback) {
    if (this.connection)
    {
      var self = this;
      models.db.DataProvider.create(dataProviderObject).then(function(dataProvider){
        self.data.dataProviders.push(dataProvider);
        callback(dataProvider);
      //  todo: emit signal
      }).catch(function(err){
        throw TypeError("Could not save DataProvider: " + err);
      });
    }
    else
      throw TypeError("DataManager not initialized yet. Could not save DataProvider");
  },

  getDataProvider: function(restriction) {
    var dataProvider = this.data.dataProviders.getItemByParam(restriction);
    return dataProvider ? dataProvider.dataValues : dataProvider;
  },

  listDataProviders: function() {
    var dataProviderObjectList = [];
    for(var index = 0; index < this.data.dataProviders.length; ++index)
      dataProviderObjectList.push(this.data.dataProviders[index].dataValues);
    return dataProviderObjectList;
  },

  removeDataProvider: function(dataProviderParam, callback) {
    var providerToBeRemoved = this.data.dataProviders.removeItem(dataProviderParam);

    if (providerToBeRemoved)
      providerToBeRemoved.destroy().then(function(status) {
        callback();
      });
    else
      throw TypeError("DataManager not initialized yet. Could not remove DataProvider");
  },

  getDataSerie: function(dataSerieValue)
  {
    return this.data.dataSeries.getItemByParam(dataSerieValue);
  },

  addDataSerie: function(dataSeriesObject) {
    this.data.dataSeries.push(dataSeriesObject);
    //tcpManager.emit(SIGNALS.DataSeriesAdded, dataSeriesObject);
  },

  updateDataSerie: function(dataSeriesObject) {
    //var dataSerie = this.data.dataSeries.getItemByParam({id: dataSeriesObject.id});
    for(var dataSerie in this.data.dataSeries)
    {
      if (dataSerie.id == dataSeriesObject.id)
      {
        dataSerie = dataSeriesObject;
        return;
      }
    }
    //  occurred error
  },

  removeDataSerie: function(dataSeriesId) {
    this.data.dataProviders.removeItem(dataSeriesId);
  }


};

module.exports = DataManager;