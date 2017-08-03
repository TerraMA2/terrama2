module.exports = function(app) {
  var controller = app.controllers.configuration.ViewsRetriever;

  app.post(app.locals.BASE_URL + 'retrieve-views', controller.retrieveViews);
  app.post(app.locals.BASE_URL + 'retrieve-removed-views', controller.retrieveRemovedViews);
  app.post(app.locals.BASE_URL + 'retrieve-notified-views', controller.retrieveNotifiedViews);
};