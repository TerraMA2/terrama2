module.exports = function (app) {
  var controller = app.controllers.index;

  app.get('/', controller);

  app.post('/userDefaults', function(request, response) {
    app.locals.collapsed = request.body.collapsed;
    return response.json({status: 200});
  });
};
