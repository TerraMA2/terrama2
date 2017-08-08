module.exports = function(app) {
  var controller = app.controllers.configuration.PredefinedStyles;

  app.get(app.locals.BASE_URL + 'predefined-styles', controller.getPredefinedStyles);
};