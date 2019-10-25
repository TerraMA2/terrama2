module.exports = function (app) {
    'use strict';

    var controller = app.controllers.api.Car;

    app.get(app.locals.BASE_URL + 'api/car/getAllSimplified', controller.getAllSimplified);
    app.get(app.locals.BASE_URL + 'api/car/getAll', controller.getAll);
};
