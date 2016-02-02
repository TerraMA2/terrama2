module.exports = function(app) {

    function staticDataController(request, response)
    {
        response.render('configuration/staticData');
    };

    return staticDataController;
};