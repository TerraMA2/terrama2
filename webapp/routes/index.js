module.exports = function (app) {
  var controller = app.controllers.index;

  app.get(app.locals.BASE_URL, controller);

  app.get(app.locals.BASE_URL + 'firstAccess', controller);

  app.post(app.locals.BASE_URL + 'userDefaults', function(request, response) {
    app.locals.collapsed = !(request.body.collapsed == "true");
    return response.json({status: 200, collapsed: app.locals.collapsed});
  });

  app.post(app.locals.BASE_URL + 'languages', function(request, response) {
    app.locals.userLocale = request.body.locale || "us";
    return response.json({status: 200});
  });

  app.delete(app.locals.BASE_URL, function(request, response) {
    response.status(400);
    response.json({status: 400, message: "You don\'t have permission"});
  });
};
