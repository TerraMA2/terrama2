// dependencies
const storageFacade = require('../../core/facade/storage');

/**
 * It exports a object with Storage controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: async (request, response) => {
      let output = null;

      const facade = new storageFacade();

      if (request.params.id) {
        output = await facade.get(request.params.id);
      } else {
        output = await facade.list();
      }

      response.json(output);
    },
    save: async (request, response) => {
      try {
        const storage = await new storageFacade().save({project_id: request.session.activeProject.id, ...request.body});
        response.json(storage);
      } catch (err) {
        response.status(err.code);
        response.json(err.getErrors());
      }
    },
  };
};
