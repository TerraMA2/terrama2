module.exports = function(app) {
  function indexController(request, response) {
    if(request.isAuthenticated() && request.user.dataValues.administrator) {
      return response.redirect("/administration/services");
    } else if(request.isAuthenticated()) {
      return response.redirect("/configuration/projects");
    } else {
      app.locals.activeProject = {};
      return response.redirect("/login");
    }
  }

  return indexController;
};
