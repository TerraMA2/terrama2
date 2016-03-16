module.exports = function (app) {

  var controller = app.controllers.api.Uri;
  app.post('/uri/', controller.post);

};