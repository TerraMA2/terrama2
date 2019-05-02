// dependencies
const storageFacade = require('../../core/facade/storage');

const mergeStorageWithProject = (request, storage) => (
  { project_id: request.session.activeProject.id, ...storage }
);

/**
 * It exports a object with Storage controllers (get/new/edit)
 * @return {Object} A object with controllers with http method as key (get/new/edit)
 */
module.exports = function(app) {
  return {
    get: async (request, response) => {
      try {
        let output = null;

        const facade = new storageFacade();

        if (request.params.id) {
          output = await facade.get(request.params.id);
        } else {
          output = await facade.list();
        }

        response.json(output);
      } catch (err) {
        response.status(400);
        response.json({ error: err.message });
      }
    },
    save: async (request, response) => {
      try {
        const storage = await new storageFacade().save(mergeStorageWithProject(request, request.body));
        response.json(storage);
      } catch (err) {
        response.status(err.code);
        response.json(err.getErrors());
      }
    },
    put: async (request, response) => {
      try {
        const facade = new storageFacade();

        await facade.update(request.params.id, mergeStorageWithProject(request, request.body));

        response.status(204);
        response.json({});
      } catch (err) {
        response.status(err.code);
        response.json(err.getErrors());
      }
    }
  };
};
