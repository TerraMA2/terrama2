module.exports = function (app) {
  var controller = app.controllers.configuration.ExistHelper;

  app.post(app.locals.BASE_URL + 'exist-helper', controller.existHelper);
}