module.exports = {
    UrlHandler: function(request, response, objectType, template)
    {
        var queryType = request.query.type;

        if (queryType != "static" && queryType != "dynamic")
        {
            response.statusCode = 404;
            // temp: It should return an default 404 template
            response.send('404. Not found');
        }
        else
        {
            response.render(template, {kind: objectType, state: queryType});
        }
    }
}