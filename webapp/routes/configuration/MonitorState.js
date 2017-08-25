module.exports = function(app) {
  var controller = app.controllers.configuration.MonitorState;

  app.post(app.locals.BASE_URL + 'get-state', controller.getState);
  app.post(app.locals.BASE_URL + 'save-state', controller.saveState);
};