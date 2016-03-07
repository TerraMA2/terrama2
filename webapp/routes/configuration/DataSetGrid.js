module.exports = function(app)
{
    var controller = app.controllers.configuration.DataSetGrid;

    app.get("/configuration/grid", controller);
}