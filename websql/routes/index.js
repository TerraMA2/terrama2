var express = require('express');
var router = express.Router();
const pg = require('pg');
const path = require('path');
const connectionString = process.env.DATABASE_URL || 'postgres://localhost:5432/todo';
var dbConnect = require("../Core/dbConnection");


/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.post('/', function(req, res){
  var queryToDb = req.body.query;
  querybox  dbConnect.doQuery(queryToDb);
})

module.exports = router;

