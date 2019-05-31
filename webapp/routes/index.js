module.exports = function (app) {
  var passport = require('./../core/utility/Passport');
  var controller = app.controllers.index;

  app.get(app.locals.BASE_URL, controller);

  app.get(app.locals.BASE_URL + 'firstAccess', controller);

  app.get(app.locals.BASE_URL + 'about', passport.isAuthenticated, function(request, response) {
    response.render('about');
  });

  app.post(app.locals.BASE_URL + 'userDefaults', function(request, response) {
    request.session.collapsed = !(request.body.collapsed == "true");
    return response.json({status: 200, collapsed: request.session.collapsed});
  });

  app.post(app.locals.BASE_URL + 'languages', function(request, response) {
    request.session.userLocale = request.body.locale || "us";
    return response.json({status: 200});
  });

  app.delete(app.locals.BASE_URL, function(request, response) {
    response.status(400);
    response.json({status: 400, message: "You don\'t have permission"});
  });
};
