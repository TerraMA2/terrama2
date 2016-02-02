module.exports = function (app) {

    var controller = app.controllers.configuration.Analysis;

    app.get('/configuration/analyses', controller.index);
    app.get('/configuration/analyse', controller["new"]);
}