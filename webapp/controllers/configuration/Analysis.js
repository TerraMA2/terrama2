module.exports = function(app)
{
    var controllers = {
        "index": function analysesController(request, response)
                {
                    response.render("configuration/analyses");
                },
        "new": function newAnalyseController(request, response)
                {
                    response.render("configuration/analyse");
                }
    }
    return controllers;
}