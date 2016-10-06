(function() {
  'use strict';

  var DataManager = require("./../DataManager");
  var Utils = require("./../Utils");
  var PromiseClass = require("bluebird");

  /**
   * It represents a mock to handle ProcessFinished events, inserting/updating object retrieved from C++ services
   * 
   * @class ProcessFinished
   */
  var ProcessFinished = module.exports = {};
  /**
   * @param {Object} registeredViewObject - A registered object retrieved from C++ services.
   * @param {string} class - A class name
   * @param {number} view_id - Determines which view were processed
   * 
   * @returns {Promise<RegisteredView>}
   */
  ProcessFinished.handleRegisteredViews = function(registeredViewObject) {
    DataManager.orm(function(t) {
      var options = {transaction: t};

      return DataManager.getRegisteredView({view_id: registeredViewObject.view_id}, options)
        // on registeredView retrieved, performs update layers
        .then(function(registeredView) {
          var promises = [];
          registeredViewObject.layers_list.forEach(function(layer) {
            promises.push(DataManager.upsertLayer(registeredView.id, {name: layer}, options));
          });
          return Promise.all(promises)
            .then(function() {
              return DataManager.getRegisteredView({view_id: registeredViewObject.view_id}, options)
            });
        })
        // NotFound... tries to insert a new one
        .catch(function(err) {
          return DataManager.addRegisteredView(registeredViewObject, options);
        });
    })

    .then(function(registeredView) {
      return resolve(registeredView);
    })

    .catch(function(err) {
      return reject(new Error(Utils.format("Error during add registered views: %s" + err.toString())));
    });
  };


} ());