'use strict';

// dependencies
var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

/**
 * It exports a object with StaticDataSeries controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      response.render('configuration/staticData', Object.assign({}, parameters, {"Enums": Enums}));
    },

    new: function(request, response) {
      response.render('configuration/dataset', {type: "static", "Enums": Enums});
    },

    edit: function(request, response) {
      var dataSeriesId = request.params.id;

      DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeriesResult) {
        response.render('configuration/dataset', {type: "static", "Enums": Enums, dataSeries: {input: dataSeriesResult.rawObject()}});
      }).catch(function(err) {
        response.render('base/404');
      })
    }
  };
};
