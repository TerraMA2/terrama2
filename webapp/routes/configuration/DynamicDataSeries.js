module.exports = function(app)
{
    var controller = app.controllers.configuration;

    app.get("/configuration/dynamic/dataseries/", controller.DynamicDataSeries);
}