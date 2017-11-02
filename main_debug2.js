var milkcocoa = new MilkCocoa('***.mlkcca.com');  // replace *** as MilkCocoa Application ID
var chatDataStore = milkcocoa.dataStore("");  // MilkCocoa Datastore Name
var v_r, v_g, v_b, v_s, v_ax, v_ay, v_az, kanpai, board;

window.onload = function(){
  v_r = document.getElementById("r");
  v_g = document.getElementById("g");
  v_b = document.getElementById("b");
  v_s = document.getElementById("s");
  v_ax = document.getElementById("ax");
  v_ay = document.getElementById("ay");
  v_az = document.getElementById("az");
  v_kanpai = document.getElementById("kanpai");
  board = document.getElementById("board");
}
 
function clickEvent(){
  var red = parseInt(v_r.value);
  var green = parseInt(v_g.value);
  var blue = parseInt(v_b.value);
  var strength = parseInt(v_s.value);
  var xaxis = parseInt(v_ax.value);
  var yaxis = parseInt(v_ay.value);
  var zaxis = parseInt(v_az.value);
  var kanpai = parseInt(v_kanpai.value);
  sendText(red, green, blue, strength, xaxis, yaxis, zaxis, kanpai);
}

function sendText(v_r, v_g, v_b, v_s, v_ax, v_ay, v_az, v_kanpai){
//  chatDataStore.push({r : v_r, g : v_g, b : v_b, s : v_s, ax : v_ax, ay : v_ay, az : v_az, kanpai : v_kanpai},function(data){
  chatDataStore.push({r : v_r, g : v_g, b : v_b, ax : v_ax, ay : v_ay, az : v_az},function(data){
    console.log("送信完了!");
  });
}
 
chatDataStore.on("push",function(data){
//  addText(data.value.r, data.value.g, data.value.b, data.value.s, data.value.ax, data.value.ay, data.value.az, data.value.kanpai);
  addText(data.value.r, data.value.g, data.value.b, data.value.ax, data.value.ay, data.value.az);
});
 
// function addText(v_r, v_g, v_b, v_s, v_ax, v_ay, v_az, v_kanpai){
function addText(v_r, v_g, v_b, v_ax, v_ay, v_az){
  var msgDom = document.createElement("li");
//  msgDom.innerHTML = v_r + "," + v_g + "," + v_b + "," + v_s + "," + v_ax + "," + v_ay + "," + v_az + "," + v_kanpai;
  msgDom.innerHTML = v_r + "," + v_g + "," + v_b + "," + v_ax + "," + v_ay + "," + v_az;
  board.insertBefore(msgDom, board.firstChild);
}


