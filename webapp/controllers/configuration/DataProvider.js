module.exports = function(app) {

    function staticDataProviderController(request, response)
    {
        // TODO: its temp code. 
        // you may set kind of provider to specific if provider is static or dynamic. i.e {kind: "dynamic"}
        var providerType = request.query.type;

        if (providerType != "static" && providerType != "dynamic")
        {
            response.statusCode = 404;
            response.send('404. Not found');
        }
        else
        {
            response.render('configuration/provider', {kind: providerType});
        }
    };

    return staticDataProviderController;
};