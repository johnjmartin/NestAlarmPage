var express = require("express");
var app = express();
var router = express.Router();
var path = __dirname + '/views/';
var paths = require('path');


router.use(function (req,res,next) {
  console.log("/" + req.method);
  next();
});

router.get("/",function(req,res){
  res.sendFile(path + "login.html");
});

router.get("/home",function(req,res){
  res.sendFile(path + "index.html");
});

router.get("/login",function(req,res){
  res.sendFile(path + "login.html");
});

router.get("/data",function(req,res){
  res.sendFile(path + "data.html");
});

router.get("/settings",function(req,res){
  res.sendFile(path + "profile.html");
});

app.use(express.static(paths.join(__dirname, '/public')));

app.use("/",router);

app.use("*",function(req,res){
  res.sendFile(path + "404.html");
});

app.listen(3000,function(){
  console.log("Live at Port 3000");
});