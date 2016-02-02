module.exports = function (app) {

    var controller = app.controllers.configuration;

    // Static DataProviders And Static DataSets (GET)
    app.get('/configuration/static-data', controller.StaticData);
}