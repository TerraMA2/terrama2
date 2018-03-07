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
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;
      response.render('configuration/staticData', Object.assign({}, parameters, {"Enums": Enums}));
    },

    new: function(request, response) {
      response.render('configuration/dataset', {type: "static", "Enums": Enums});
    },

    edit: function(request, response) {
      var dataSeriesId = request.params.id;
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;

      DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeriesResult) {
        response.render('configuration/dataset', {type: "static", "Enums": Enums, dataSeries: {input: dataSeriesResult.rawObject()}, hasProjectPermission: hasProjectPermission});
      }).catch(function(err) {
        response.render('base/404');
      })
    },
    changeStatus: function(request, response) {
      DataManager.changeDataSeriesStatus({ id: parseInt(request.params.id) }).then(function() {
        return response.json({});
      }).catch(function(err) {
        response.status(500);
        return response.json({ err: err });
      });
    }
  };
};
