"use strict";

var DataManager = require("../../core/DataManager");

module.exports = function(app) {
  return {
    get: function(request, response) {
      var output = [];
      DataManager.listStorages({ DataProvider: { storage_id: storage } }).then(function(storages) {
          storages.forEach(function(element) {
          output.push(element.rawObject());
        });
        response.json(output);
      });
    }
  };
};
