
var str = "nextval('focos_408_pid_focos_408_seq'::regclass)";
var old = "focos_408";
var new1 = "back_queimadas408_a";
var re = new RegExp(old, 'g');

str = str.replace(re, new1);
console.log(str);