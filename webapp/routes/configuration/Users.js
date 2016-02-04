module.exports = function (app) {

    var controller = app.controllers.configuration.Users;

    app.get('/configuration/users', controller.index);
    app.get('/configuration/user', controller["new"]);
}