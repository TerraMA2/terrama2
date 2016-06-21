var EventEmitter = require('events').EventEmitter;
var NodeUtils = require('util');

function A() {
  EventEmitter.call(this);
}

NodeUtils.inherits(A, EventEmitter);


var a = new A();

function connection() {
  console.log("connected");
}

a.on('connect', connection)

a.emit('connect');

a.removeListener('connect', connection);
