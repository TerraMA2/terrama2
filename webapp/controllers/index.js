module.exports = function(app) {

  function indexController(request, response) {
    app.locals.activeProject = {};
    app.locals.user = {};
    return response.redirect("/login");
  }

  return indexController;
};
