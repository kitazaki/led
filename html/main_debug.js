var milkcocoa = new MilkCocoa('***.mlkcca.com');  // replace *** as MilkCocoa Application ID
var chatDataStore = milkcocoa.dataStore("");  // MilkCocoa Datastore Name
var v_r, v_g, v_b, v_s, board;

window.onload = function(){
  v_r = document.getElementById("r");
  v_g = document.getElementById("g");
  v_b = document.getElementById("b");
  v_s = document.getElementById("s");
  board = document.getElementById("board");
}
 
function clickEvent(){
  var red = parseInt(v_r.value);
  var green = parseInt(v_g.value);
  var blue = parseInt(v_b.value);
  var strength = parseInt(v_s.value);
  sendText(red, green, blue, strength);
}

function sendText(v_r, v_g, v_b, v_s){
  chatDataStore.push({r : v_r, g : v_g, b : v_b, s : v_s},function(data){
    console.log("送信完了!");
  });
}
 
chatDataStore.on("push",function(data){
  addText(data.value.r, data.value.g, data.value.b, data.value.s);
});
 
function addText(v_r, v_g, v_b, v_s){
  var msgDom = document.createElement("li");
  msgDom.innerHTML = v_r + "," + v_g + "," + v_b + "," + v_s;
  board.insertBefore(msgDom, board.firstChild);
}

