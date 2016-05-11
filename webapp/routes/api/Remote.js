module.exports = function(app) {
  var controller = app.controllers.api.Remote;

  app.post("/api/Remote", controller.post);
  app.post("/api/Remote/ping", controller.ping);
}