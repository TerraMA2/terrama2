module.exports = function (app) {
    'use strict';

    var controller = app.controllers.api.Projus;

    app.get(app.locals.BASE_URL + 'api/projus/getAll', controller.getAll);
};
