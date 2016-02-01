module.exports = function (app) {

    var controller = app.controllers.configuration;

    app.get('/configuration/analyses', controller.analyse);
}