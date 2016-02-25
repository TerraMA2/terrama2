module.exports = function(app) {

    function dynamicDataSeriesController(request, response)
    {
        response.render('configuration/dynamicDataSeries');
    };

    return dynamicDataSeriesController;
};