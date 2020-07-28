
module.exports = (app) => {
  let passport = require('../../core/utility/Passport');
  let controller = app.controllers.api.Cemaden;

  app.get(app.locals.BASE_URL + 'api/Cemaden/', passport.isAuthenticated, controller.list);
  app.get(app.locals.BASE_URL + 'api/Cemaden/stations', passport.isAuthenticated, controller.listStations);
};
