'use strict';

// dependencies
var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

/**
 * It exports a object with Storage controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;
      response.render('configuration/storages', Object.assign({}, parameters, {"Enums": Enums}));
    },

    new: function(request, response) {
      response.render('configuration/storage');
    },

    edit: function(request, response) {
      var storageId = request.params.id;
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;

      DataManager.getDataSeries({id: storageId}).then(function() {
        response.render('configuration/storage', {hasProjectPermission: hasProjectPermission});
      }).catch(function(err) {
        response.render('base/404');
      })
    }
  };
};
