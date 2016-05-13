module.exports = function(app) {
  var controller = app.controllers.api.Remote;

  app.post("/api/Remote/start", controller.startService);
  app.post("/api/Remote/stop", controller.stopService);
}