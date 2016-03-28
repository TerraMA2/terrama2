var DataManager = require('./DataManager');

module.exports = {
  projectRequired: function(request, response, next) {
    if (request.locals.activeProject) {
      DataManager.getProject({name: request.locals.activeProject}).then(function() {
        next();
      }).catch(function(err) {
        redirect("/configuration/projects");
      });
    }

    redirect("/configuration/projects");
  }
};