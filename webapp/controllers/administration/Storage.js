'use strict';

// dependencies
const Enums = require('./../../core/Enums');
const StorageFacade = require('./../../core/facade/storage');
const makeTokenParameters = require('../../core/Utils').makeTokenParameters;

/**
 * It exports a object with Storage controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: function(request, response) {
      const parameters = makeTokenParameters(request.query.token, app);
      const hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;
      response.render('administration/storages', Object.assign({}, parameters, {"Enums": Enums}));
    },

    new: function(request, response) {
      response.render('administration/storage');
    },

    edit: async (request, response) => {
      const storageId = request.params.id;
      const hasProjectPermission = request.session.activeProject.hasProjectPermission;

      const facade = new StorageFacade();

      try {
        await facade.get(storageId);

        response.render('administration/storage', {hasProjectPermission: hasProjectPermission, storage: storageId });
      } catch (err) {
        response.render('base/404');
      }
    }
  };
};
