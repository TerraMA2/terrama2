'use strict';

// dependencies
var storageFacade = require('../../core/facade/storage');

/**
 * It exports a object with Storage controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: async (request, response) => {
      const storages = await new storageFacade().list();
      response.json(storages);
    },
    save: async (request, response) => {
      const storage = await new storageFacade().save(request.body);
      response.json(storage);
    },
  };
};
