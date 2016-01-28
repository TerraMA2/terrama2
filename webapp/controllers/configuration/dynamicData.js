module.exports = function(app) {

    function dynamicDataController(request, response)
    {
        response.render('configuration/dynamicData');
    };

    return dynamicDataController;
};