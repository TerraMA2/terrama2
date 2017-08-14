module.exports = function(app) {
  var controller = app.controllers.CheckGridController;

  app.post(app.locals.BASE_URL + 'check-grid', controller.checkGridFile);
  app.post(app.locals.BASE_URL + 'check-grid-folder', controller.checkGridFolder);
};