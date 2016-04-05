module.exports = function (app) {

  var controller = app.controllers.configuration.MapDisplay;

  app.get('/configuration/mapdisplay', controller.get);
};