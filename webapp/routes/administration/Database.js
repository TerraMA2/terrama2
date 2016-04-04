module.exports = function (app) {

  var controller = app.controllers.administration.Database;
  
  app.get('/administration/databases/new', controller.new);

};