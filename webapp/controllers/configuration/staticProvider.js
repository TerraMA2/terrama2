module.exports = function(app) {

    function staticDataProviderController(request, response)
    {
        // you may set kind of provider to specific if provider is static or dynamic. i.e {kind: "dynamic"}
        response.render('configuration/provider');
    };

    return staticDataProviderController;
};