module.exports = function(app) {

    function staticDataProviderController(request, response)
    {
        response.render('configuration/staticProvider');
    };

    return staticDataProviderController;
};