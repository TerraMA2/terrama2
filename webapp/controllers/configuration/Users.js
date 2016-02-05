module.exports = function(app)
{
    var controllers = {
        "index": function usersController(request, response)
                {
                    response.render("configuration/users");
                },
        "new": function newUserController(request, response)
                {
                    response.render("configuration/user");
                }
    }
    return controllers;
}