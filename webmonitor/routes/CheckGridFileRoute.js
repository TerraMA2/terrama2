module.exports = function(app) {
  var controller = app.controllers.CheckGridFileController;

  app.post(app.locals.BASE_URL + 'check-grid', controller);
};