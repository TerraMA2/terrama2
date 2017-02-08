'use strict';

module.exports = function(app) {
  var passport = require('./../config/Passport');
  var Utils = require("./../core/Utils");

  app.get("/profile", passport.isAuthenticated, function(request, response) {
    var parameters = Utils.makeTokenParameters(request.query.token, app);
    return response.render("profile", Object.assign({
      update: true,
      currentTab: "profile",
      profile: true,
      redirectUrl: "/configuration/projects/"
    }, parameters));
  });
};