module.exports = function (app) {

    var controller = app.controllers.configuration;

    // Dynamic DataProviders And Dynamic DataSets (GET)
    app.get('/configuration/dynamic-data', controller.dynamicData);
}