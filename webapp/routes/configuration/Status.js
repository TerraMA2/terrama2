module.exports = function(app) {
  var controller = app.controllers.configuration.Status;
  
  app.get("/configuration/status", controller);
};